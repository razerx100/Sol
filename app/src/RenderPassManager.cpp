#include <RenderPassManager.hpp>
#include <stack>
#include <BasicMeshBundles.hpp>

void RenderPassManager::SetRenderer(Renderer* renderer) noexcept
{
	m_renderer        = renderer;

	m_resourceFactory = m_renderer->GetExternalResourceManager()->GetResourceFactory();
}

void RenderPassManager::CreateResources()
{
	m_mainRenderTargetIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_mainRenderTarget      = m_resourceFactory->GetExternalTextureSP(m_mainRenderTargetIndex);

	m_depthTargetIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_depthTarget      = m_resourceFactory->GetExternalTextureSP(m_depthTargetIndex);
}

std::uint32_t RenderPassManager::SetupMainDrawingPass()
{
	const std::uint32_t renderTargetIndex = m_mainPass->AddRenderTarget(
		m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_mainPass->SetRenderTargetClearColour(
		renderTargetIndex, DirectX::XMFLOAT4{ 0.005f, 0.005f, 0.005f, 1.f }
	);

	m_mainPass->SetDepthTesting(
		m_depthTargetIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_mainPass->SetDepthClearColour(1.f);

	SetupMainPassSignatures();

	return renderTargetIndex;
}

std::uint32_t RenderPassManager::SetupPostProcessingPass()
{
	const std::uint32_t renderTargetIndex = m_postProcessingPass->AddRenderTarget(
		m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Load, ExternalAttachmentStoreOp::Store
	);

	SetupPostProcessingSignatures();

	// Since in the post processing pass, our main purpose is to combine/apply effects to the already
	// drawn main renderTarget, we would only need to draw a quad of the same extent as the render target.
	// So, we can just add that model here.
	m_postProcessingPass->AddModelBundle(m_renderTargetQuadModelBundleIndex);

	return renderTargetIndex;
}

void RenderPassManager::SetupMainPassSignatures()
{
	ExternalGraphicsPipeline opaqueSignature{};

	opaqueSignature.EnableBackfaceCulling();

	opaqueSignature.AddRenderTarget(
		m_renderer->GetSwapchainFormat(), ExternalBlendState { .enabled = false }
	);

	opaqueSignature.EnableDepthTesting(s_renderDepthFormat, true);

	m_graphicsPipelineManager.SetMainPassOpaqueSignature(std::move(opaqueSignature));
}

void RenderPassManager::SetupPostProcessingSignatures()
{
	if (m_transparencyExt)
		m_transparencyExt->SetupTransparencyGraphicsPipelineSignatures(
			m_graphicsPipelineManager, s_renderDepthFormat
		);
}

void RenderPassManager::SetupRenderPassesFromRenderer()
{
	// Need to setup here, as adding meshes/models require the descriptor heaps/buffer to be created first.
	SetupRenderTargetQuad();

	const bool hasTransparencyPass = m_transparencyExt != nullptr;

	bool hasPostProcessingPass     = false;

	hasPostProcessingPass = hasTransparencyPass;

	// Total passes minus one, because we need to setup the swapchain one separately.
	size_t totalPassCount = 0u;

	if (hasTransparencyPass)
		totalPassCount += m_transparencyExt->GetExtraRequiredPassCount();

	if (hasPostProcessingPass)
		++totalPassCount;

	std::vector<std::shared_ptr<ExternalRenderPass>> renderPasses{};

	renderPasses.reserve(totalPassCount);

	for (size_t index = 0u; index < totalPassCount; ++index)
	{
		const size_t renderPassIndex = m_renderer->AddExternalRenderPass();

		renderPasses.emplace_back(m_renderer->GetExternalRenderPassSP(renderPassIndex));
	}

	// The swapchain should be the last pass.
	m_renderer->SetSwapchainExternalRenderPass();

	std::shared_ptr<ExternalRenderPass> swapchainPass = m_renderer->GetSwapchainExternalRenderPassSP();

	const bool isMainPassSwapchainSource = !hasPostProcessingPass;

	if (!isMainPassSwapchainSource)
	{
		// Should be the first one until I add a Z pass.
		size_t mainPassIndex         = 0u;
		size_t transparencyPassIndex = 1u;

		m_mainPass = std::move(renderPasses[mainPassIndex]);

		if (hasTransparencyPass)
			m_transparencyExt->SetTransparencyPass(renderPasses[transparencyPassIndex]);

		if (hasPostProcessingPass)
			m_postProcessingPass = swapchainPass;
	}
	else
		m_mainPass = swapchainPass;

	std::uint32_t swapchainCopySourceIndex = SetupMainDrawingPass();

	if (hasPostProcessingPass)
		swapchainCopySourceIndex = SetupPostProcessingPass();

	if (hasTransparencyPass)
	{
		m_transparencyExt->SetupTransparencyPass(m_depthTargetIndex);

		m_transparencyExt->SetupCompositePassPipeline(
			m_postProcessingPass.get(), m_graphicsPipelineManager
		);
	}

	swapchainPass->SetSwapchainCopySource(swapchainCopySourceIndex);
}

void RenderPassManager::Resize()
{
	m_renderer->WaitForGPUToFinish();

	Renderer::Extent renderArea = m_renderer->GetCurrentRenderingExtent();

	m_mainRenderTarget->Create(
		renderArea.width, renderArea.height, m_renderer->GetSwapchainFormat(),
		ExternalTexture2DType::RenderTarget, true, false
	);

	m_depthTarget->Create(
		renderArea.width, renderArea.height, s_renderDepthFormat, ExternalTexture2DType::Depth,
		false, false
	);

	m_mainPass->ResetAttachmentReferences();

	if (m_transparencyExt)
		m_transparencyExt->ResizeRenderTargets(renderArea.width, renderArea.height);
}

void RenderPassManager::ResizeCallback([[maybe_unused]] void* callbackData, void* extraData)
{
	auto sThis = static_cast<RenderPassManager*>(extraData);

	sThis->Resize();
}

void RenderPassManager::SetupRenderTargetQuad()
{
	// For some passes we would just want to combine or do some operation on an already
	// rendered texture. So, we wouldn't want to draw anything but we still need to draw something
	// to invoke the fragment shaders. So, we will bind the necessary render targets as textures
	// and a quad with the same extent as the render targets and the fragment shaders of that would
	// combine or do some other stuff.
	Mesh mesh{};

	QuadMesh{}.SetMesh(mesh);

	MeshBundleImpl quadMesh{ true };

	quadMesh.AddMesh(std::move(mesh));

	m_quadMeshBundleIndex = m_renderer->AddMeshBundle(quadMesh.MoveTemporaryData());

	m_renderTargetQuadModelBundle = std::make_shared<ModelBundleBase>();

	m_renderTargetQuadModelBundle->AddModel(2.f);

	{
		ModelBase& quadModel = *m_renderTargetQuadModelBundle->GetModel(0u);

		quadModel.SetMeshIndex(0u);
	}

	m_renderTargetQuadModelBundle->SetMeshBundleIndex(m_quadMeshBundleIndex);

	m_renderTargetQuadModelBundleIndex = m_renderer->AddModelBundle(
		std::make_shared<ModelBundleImpl>(m_renderTargetQuadModelBundle)
	);
}

void RenderPassManager::SetTransparencyPass(
	std::shared_ptr<WeightedTransparencyTechnique> transparencyExt
) {
	m_transparencyExt = std::move(transparencyExt);
}

void RenderPassManager::SetPostCreationReferences()
{
	if (m_transparencyExt)
		// Need to bind the two render targets to combine in post-processing.
		m_transparencyExt->BindRenderTargetTextures();
}
