#ifndef RENDER_PASS_MANAGER_HPP_
#define RENDER_PASS_MANAGER_HPP_
#include <memory>
#include <utility>
#include <ExternalBuffer.hpp>
#include <ExternalRenderPass.hpp>
#include <ExternalResourceFactory.hpp>
#include <GraphicsPipelineManager.hpp>
#include <WeightedTransparencyTechnique.hpp>
#include <ModelBase.hpp>

#include <BasicMeshBundles.hpp>
#include <RendererCommonTypes.hpp>

class RenderPassManager
{
	using TransparencyExt_t = std::shared_ptr<WeightedTransparencyTechnique>;

public:
	RenderPassManager(RenderEngineType engineType)
		: m_mainPass{}, m_postProcessingPass{}, m_mainRenderTarget{}, m_depthTarget{},
		m_mainRenderTargetIndex{ 0u }, m_depthTargetIndex{ 0u }, m_quadMeshBundleIndex{ 0u },
		m_renderTargetQuadModelBundleIndex{ 0u }, m_renderTargetQuadModelBundle{},
		m_transparencyExt{}, m_graphicsPipelineManager{ engineType },
		m_resourceFactory{ nullptr }
	{}

	void AddPipeline(std::uint32_t pipelineIndex)
	{
		m_mainPass->AddPipeline(pipelineIndex);
	}

	void AddModelBundle(std::uint32_t bundleIndex)
	{
		m_mainPass->AddModelBundle(bundleIndex);
	}

	void RemoveModelBundle(std::uint32_t bundleIndex) noexcept
	{
		m_mainPass->RemoveModelBundle(bundleIndex);
	}
	void RemovePipeline(std::uint32_t pipelineIndex) noexcept
	{
		m_mainPass->RemovePipeline(pipelineIndex);
	}

	void SetTransparencyPass(std::shared_ptr<WeightedTransparencyTechnique> transparencyExt);

	template<class Renderer_t>
	void SetResourceFactory(Renderer_t& renderer) noexcept
	{
		m_resourceFactory = renderer.GetExternalResourceManager()->GetResourceFactory();
	}

	void CreateResources();

	template<class Renderer_t>
	void SetupRenderPassesFromRenderer(Renderer_t& renderer)
	{
		// Need to setup here, as adding meshes/models require the descriptor heaps/buffer to be
		// created first.
		SetupRenderTargetQuad(renderer);

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
			const size_t renderPassIndex = renderer.AddExternalRenderPass();

			renderPasses.emplace_back(renderer.GetExternalRenderPassSP(renderPassIndex));
		}

		// The swapchain should be the last pass.
		renderer.SetSwapchainExternalRenderPass();

		std::shared_ptr<ExternalRenderPass> swapchainPass
			= renderer.GetSwapchainExternalRenderPassSP();

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

		std::uint32_t swapchainCopySourceIndex = SetupMainDrawingPass(renderer);

		if (hasPostProcessingPass)
			swapchainCopySourceIndex = SetupPostProcessingPass(renderer);

		if (hasTransparencyPass)
		{
			m_transparencyExt->SetupTransparencyPass(m_depthTargetIndex);

			m_transparencyExt->SetupTransparencyGraphicsPipelineSignatures(
				m_graphicsPipelineManager, s_renderDepthFormat
			);

			m_transparencyExt->SetCompositePass(m_postProcessingPass);

			m_transparencyExt->SetupCompositePassPipeline(
				m_postProcessingPass.get(), m_graphicsPipelineManager,
				*m_renderTargetQuadModelBundle, s_renderTargetQuadMeshIndex,
				renderer
			);
		}

		// This must be done at the end so all the other passes using the render target quad
		// modelBundle can add their instance of the quad model with their own pipeline.
		// And we also must add the model bundle to the renderer before adding them to the render
		// passes.
		if (!std::empty(m_renderTargetQuadModelBundle->GetModels()))
			m_renderTargetQuadModelBundleIndex = renderer.AddModelBundle(
				std::make_shared<ModelBundleImpl>(m_renderTargetQuadModelBundle)
			);

		// This must be done at the end so all the other passes using the post processing pass
		// can add their pipelines before we add the model bundle to the pass.
		if (hasPostProcessingPass)
			AddRenderTargetQuadToPostProcessing();

		swapchainPass->SetSwapchainCopySource(swapchainCopySourceIndex);
	}

	// The render area might be a bit
	// smaller because of the title or something else, so we need to grab the render area from
	// the renderer.
	template<class Renderer_t>
	void Resize(this RenderPassManager& self, Renderer_t& renderer)
	{
		RendererType::Extent renderArea = renderer.GetCurrentRenderingExtent();

		self.m_mainRenderTarget->Create(
			renderArea.width, renderArea.height, renderer.GetSwapchainFormat(),
			ExternalTexture2DType::RenderTarget, ExternalTextureCreationFlags{ .copySrc = true }
		);

		self.m_depthTarget->Create(
			renderArea.width, renderArea.height, s_renderDepthFormat,
			ExternalTexture2DType::Depth
		);

		self.m_mainPass->ResetAttachmentReferences();

		if (self.m_transparencyExt)
			self.m_transparencyExt->ResizeRenderTargets(
				renderArea.width, renderArea.height, renderer
			);

		if (self.m_postProcessingPass)
			self.m_postProcessingPass->ResetAttachmentReferences();
	}

	[[nodiscard]]
	const GraphicsPipelineManager& GetGraphicsPipelineManager() const noexcept
	{
		return m_graphicsPipelineManager;
	}

	[[nodiscard]]
	std::uint32_t GetQuadMeshBundleIndex() const noexcept { return m_quadMeshBundleIndex; }

private:
	static constexpr ExternalFormat s_renderDepthFormat = ExternalFormat::D32_FLOAT;

private:
	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t SetupMainDrawingPass(Renderer_t& renderer)
	{
		const std::uint32_t renderTargetIndex = m_mainPass->AddRenderTarget(
			m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store
		);

		m_mainPass->SetRenderTargetClearColour(
			renderTargetIndex, DirectX::XMFLOAT4{ 0.005f, 0.005f, 0.005f, 1.f }
		);

		m_mainPass->SetDepthTesting(
			m_depthTargetIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store
		);

		m_mainPass->SetDepthClearColour(1.f);

		SetupMainPassSignatures(renderer);

		return renderTargetIndex;
	}

	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t SetupPostProcessingPass(Renderer_t& renderer)
	{
		const std::uint32_t renderTargetIndex = m_postProcessingPass->AddRenderTarget(
			m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Load,
			ExternalAttachmentStoreOp::Store
		);

		SetupPostProcessingSignatures(renderer);

		return renderTargetIndex;
	}

	template<class Renderer_t>
	void SetupRenderTargetQuad(Renderer_t& renderer)
	{
		// For some passes we would just want to combine or do some operation on an already
		// rendered texture. So, we wouldn't want to draw anything but we still need to draw
		// something to invoke the fragment shaders. So, we will bind the necessary render
		// targets as textures and a quad with the same extent as the render targets and the
		// fragment shaders of that would combine or do some other stuff.
		Mesh mesh{};

		RenderTargetQuadMesh{}.SetMesh(mesh);

		MeshBundleImpl quadMesh{ true };

		quadMesh.AddMesh(std::move(mesh));

		m_quadMeshBundleIndex = renderer.AddMeshBundle(quadMesh.MoveTemporaryData());

		m_renderTargetQuadModelBundle = std::make_shared<ModelBundleBase>();

		m_renderTargetQuadModelBundle->SetMeshBundleIndex(m_quadMeshBundleIndex);
	}

	template<class Renderer_t>
	void SetupMainPassSignatures(Renderer_t& renderer)
	{
		ExternalGraphicsPipeline opaqueSignature{};

		opaqueSignature.EnableBackfaceCulling();

		opaqueSignature.AddRenderTarget(
			renderer.GetSwapchainFormat(), ExternalBlendState { .enabled = false }
		);

		opaqueSignature.EnableDepthTesting(s_renderDepthFormat, true);

		m_graphicsPipelineManager.SetMainPassOpaqueSignature(std::move(opaqueSignature));
	}

	template<class Renderer_t>
	void SetupPostProcessingSignatures(Renderer_t& renderer)
	{
		if (m_transparencyExt)
			m_transparencyExt->SetupTransparencyCompositePipelineSignature(
				m_graphicsPipelineManager, renderer
			);
	}

	void AddRenderTargetQuadToPostProcessing();

private:
	std::shared_ptr<ExternalRenderPass> m_mainPass;
	std::shared_ptr<ExternalRenderPass> m_postProcessingPass;

	std::shared_ptr<ExternalTexture>    m_mainRenderTarget;
	std::shared_ptr<ExternalTexture>    m_depthTarget;

	std::uint32_t                       m_mainRenderTargetIndex;
	std::uint32_t                       m_depthTargetIndex;

	std::uint32_t                       m_quadMeshBundleIndex;

	std::uint32_t                       m_renderTargetQuadModelBundleIndex;
	std::shared_ptr<ModelBundleBase>    m_renderTargetQuadModelBundle;

	TransparencyExt_t                   m_transparencyExt;

	GraphicsPipelineManager             m_graphicsPipelineManager;

	ExternalResourceFactory*            m_resourceFactory;

	static constexpr std::uint32_t s_renderTargetQuadMeshIndex = 0u;

public:
	RenderPassManager(const RenderPassManager&) = delete;
	RenderPassManager& operator=(const RenderPassManager&) = delete;

	RenderPassManager(RenderPassManager&& other) noexcept
		: m_mainPass{ std::move(other.m_mainPass) },
		m_postProcessingPass{ std::move(other.m_postProcessingPass) },
		m_mainRenderTarget{ std::move(other.m_mainRenderTarget) },
		m_depthTarget{ std::move(other.m_depthTarget) },
		m_mainRenderTargetIndex{ other.m_mainRenderTargetIndex },
		m_depthTargetIndex{ other.m_depthTargetIndex },
		m_quadMeshBundleIndex{ other.m_quadMeshBundleIndex },
		m_renderTargetQuadModelBundleIndex{ other.m_renderTargetQuadModelBundleIndex },
		m_renderTargetQuadModelBundle{ std::move(other.m_renderTargetQuadModelBundle) },
		m_transparencyExt{ std::move(other.m_transparencyExt) },
		m_graphicsPipelineManager{ std::move(other.m_graphicsPipelineManager) },
		m_resourceFactory{ std::exchange(other.m_resourceFactory, nullptr) }
	{}
	RenderPassManager& operator=(RenderPassManager&& other) noexcept
	{
		m_mainPass                         = std::move(other.m_mainPass);
		m_postProcessingPass               = std::move(other.m_postProcessingPass);
		m_mainRenderTarget                 = std::move(other.m_mainRenderTarget);
		m_depthTarget                      = std::move(other.m_depthTarget);
		m_mainRenderTargetIndex            = other.m_mainRenderTargetIndex;
		m_depthTargetIndex                 = other.m_depthTargetIndex;
		m_quadMeshBundleIndex              = other.m_quadMeshBundleIndex;
		m_renderTargetQuadModelBundleIndex = other.m_renderTargetQuadModelBundleIndex;
		m_renderTargetQuadModelBundle      = std::move(other.m_renderTargetQuadModelBundle);
		m_transparencyExt                  = std::move(other.m_transparencyExt);
		m_graphicsPipelineManager          = std::move(other.m_graphicsPipelineManager);
		m_resourceFactory                  = std::exchange(other.m_resourceFactory, nullptr);

		return *this;
	}
};
#endif
