#include <RenderPassManager.hpp>

void RenderPassManager::SetRenderer(Renderer* renderer) noexcept
{
	m_renderer        = renderer;

	m_resourceFactory = m_renderer->GetExternalResourceManager()->GetResourceFactory();
}

void RenderPassManager::CreateAttachments()
{
	m_renderTargetIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_renderTarget      = m_resourceFactory->GetExternalTextureSP(m_renderTargetIndex);

	m_depthBufferIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_depthTarget      = m_resourceFactory->GetExternalTextureSP(m_depthBufferIndex);
}

void RenderPassManager::SetupRenderPass()
{
	m_renderer->SetSwapchainExternalRenderPass();

	m_renderPass = m_renderer->GetSwapchainExternalRenderPassSP();

	const std::uint32_t renderTargetIndex = m_renderPass->AddRenderTarget(
		m_renderTargetIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::Store
	);

	m_renderPass->SetRenderTargetClearColour(
		renderTargetIndex, DirectX::XMFLOAT4{ 0.005f, 0.005f, 0.005f, 1.f }
	);

	m_renderPass->SetDepthTesting(
		m_depthBufferIndex, ExternalAttachmentLoadOp::Clear, ExternalAttachmentStoreOp::DontCare
	);

	m_renderPass->SetDepthClearColour(1.f);

	m_renderPass->SetSwapchainCopySource(renderTargetIndex);

	SetupRenderPipelineManager();
}

void RenderPassManager::SetupRenderPipelineManager()
{
	m_renderPipelineManager.AddRenderTarget(m_renderer->GetSwapchainFormat());

	m_renderPipelineManager.SetDepthTarget(s_renderDepthFormat);
}

void RenderPassManager::SetupRenderPassesFromRenderer()
{
	SetupRenderPass();
}

void RenderPassManager::Resize()
{
	m_renderer->WaitForGPUToFinish();

	Renderer::Extent renderArea = m_renderer->GetCurrentRenderingExtent();

	m_renderTarget->Create(
		renderArea.width, renderArea.height, m_renderer->GetSwapchainFormat(),
		ExternalTexture2DType::RenderTarget, true, false
	);

	m_depthTarget->Create(
		renderArea.width, renderArea.height, s_renderDepthFormat, ExternalTexture2DType::Depth,
		false, false
	);

	m_renderPass->ResetAttachmentReferences();
}

void RenderPassManager::ResizeCallback([[maybe_unused]] void* callbackData, void* extraData)
{
	auto sThis = static_cast<RenderPassManager*>(extraData);

	sThis->Resize();
}
