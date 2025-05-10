#ifndef RENDER_PASS_MANAGER_HPP_
#define RENDER_PASS_MANAGER_HPP_
#include <memory>
#include <utility>
#include <ExternalBuffer.hpp>
#include <ExternalRenderPass.hpp>
#include <GraphicsPipelineManager.hpp>
#include <WeightedTransparencyTechnique.hpp>
#include <ModelBase.hpp>

#include <BasicMeshBundles.hpp>
#include <RendererCommonTypes.hpp>

namespace Sol
{
template<class ExternalRenderPassImpl_t, class ExternalBufferImpl_t, class ExternalTextureImpl_t>
class RenderPassManager
{
public:
	using WeightedTransparency_t = WeightedTransparencyTechnique<
		ExternalRenderPassImpl_t, ExternalBufferImpl_t, ExternalTextureImpl_t
	>;
	using TransparencyExt_t    = std::shared_ptr<WeightedTransparency_t>;
	using ExternalRenderPass_t = ExternalRenderPass<ExternalRenderPassImpl_t>;
	using ExternalTexture_t    = ExternalTexture<ExternalTextureImpl_t>;

public:
	RenderPassManager(RenderEngineType engineType)
		: m_mainPass{}, m_postProcessingPass{},
		m_mainPassIndex{ std::numeric_limits<std::uint32_t>::max() },
		m_postProcessingPassIndex{ std::numeric_limits<std::uint32_t>::max() },
		m_mainRenderTarget{}, m_depthTarget{}, m_mainPassCameraIndex{ 0u },
		m_mainRenderTargetIndex{ 0u }, m_depthTargetIndex{ 0u }, m_quadMeshBundleIndex{ 0u },
		m_renderTargetQuadModelBundleIndex{ 0u }, m_renderTargetQuadModelBundle{},
		m_transparencyExt{}, m_graphicsPipelineManager{ engineType }
	{}

	void AddPipeline(std::uint32_t pipelineIndex)
	{
		m_mainPass.AddPipeline(pipelineIndex);
	}

	template<class Renderer_t>
	void AddModelBundle(std::uint32_t bundleIndex, Renderer_t& renderer) const
	{
		renderer.AddLocalPipelinesInExternalRenderPass(
			bundleIndex, m_mainPassIndex
		);
	}

	void RemoveModelBundle(std::uint32_t bundleIndex) noexcept
	{
		m_mainPass.RemoveModelBundle(bundleIndex);
	}
	void RemovePipeline(std::uint32_t pipelineIndex) noexcept
	{
		m_mainPass.RemovePipeline(pipelineIndex);
	}

	void SetTransparencyPass(TransparencyExt_t transparencyExt)
	{
		m_transparencyExt = std::move(transparencyExt);
	}

	template<class ResourceFactory_t>
	void CreateResources(ResourceFactory_t& resourceFactory)
	{
		m_mainRenderTargetIndex = static_cast<std::uint32_t>(
			resourceFactory.CreateExternalTexture()
		);

		m_mainRenderTarget.SetTextureImpl(
			resourceFactory.GetExternalTextureSP(m_mainRenderTargetIndex)
		);

		m_depthTargetIndex = static_cast<std::uint32_t>(resourceFactory.CreateExternalTexture());

		m_depthTarget.SetTextureImpl(
			resourceFactory.GetExternalTextureSP(m_depthTargetIndex)
		);
	}

	template<class Renderer_t>
	void SetupRenderPassesFromRenderer(
		Renderer_t& renderer, CameraManager& cameraManager,
		std::shared_ptr<ModelContainer> modelContainer
	) {
		SetupCameras(cameraManager);
		// Need to setup here, as adding meshes/models require the descriptor heaps/buffer to be
		// created first.
		SetupRenderTargetQuad(renderer, std::move(modelContainer));

		const bool hasTransparencyPass = m_transparencyExt != nullptr;

		bool hasPostProcessingPass     = false;

		hasPostProcessingPass = hasTransparencyPass;

		// Total passes minus one, because we need to setup the swapchain one separately.
		size_t totalPassCount = 0u;

		if (hasTransparencyPass)
			totalPassCount += m_transparencyExt->GetExtraRequiredPassCount();

		if (hasPostProcessingPass)
			++totalPassCount;

		struct ExternalRenderPassData
		{
			std::shared_ptr<ExternalRenderPassImpl_t> renderPass;
			std::uint32_t                             renderPassIndex;
		};

		std::vector<ExternalRenderPassData> renderPasses{};

		renderPasses.reserve(totalPassCount);

		for (size_t index = 0u; index < totalPassCount; ++index)
		{
			const size_t renderPassIndex = renderer.AddExternalRenderPass();

			renderPasses.emplace_back(
				ExternalRenderPassData
				{
					.renderPass      = renderer.GetExternalRenderPassSP(renderPassIndex),
					.renderPassIndex = static_cast<std::uint32_t>(renderPassIndex)
				}
			);
		}

		// The swapchain should be the last pass.
		renderer.SetSwapchainExternalRenderPass();

		std::shared_ptr<ExternalRenderPassImpl_t> swapchainPass
			= renderer.GetSwapchainExternalRenderPassSP();

		const bool isMainPassSwapchainSource = !hasPostProcessingPass;

		if (!isMainPassSwapchainSource)
		{
			// Should be the first one until I add a Z pass.
			size_t mainPassIndex         = 0u;
			size_t transparencyPassIndex = 1u;

			ExternalRenderPassData& mainPassData = renderPasses[mainPassIndex];

			m_mainPass.SetRenderPassImpl(std::move(mainPassData.renderPass));

			m_mainPassIndex = mainPassData.renderPassIndex;

			if (hasTransparencyPass)
			{
				ExternalRenderPassData& transparencyPassData = renderPasses[transparencyPassIndex];

				m_transparencyExt->SetTransparencyPass(
					std::move(transparencyPassData.renderPass),
					transparencyPassData.renderPassIndex
				);
			}

			if (hasPostProcessingPass)
			{
				m_postProcessingPass.SetRenderPassImpl(swapchainPass);

				// Since the swapchain pass doesn't need an index, no need to set it.
			}
		}
		else
		{
			m_mainPass.SetRenderPassImpl(swapchainPass);
			// Since the swapchain pass doesn't need an index, no need to set it.
		}

		std::uint32_t swapchainCopySourceIndex = SetupMainDrawingPass(renderer);

		if (hasPostProcessingPass)
			swapchainCopySourceIndex = SetupPostProcessingPass(renderer);

		if (hasTransparencyPass)
		{
			auto& resourceFactory = renderer.GetExternalResourceManager().GetResourceFactory();

			m_transparencyExt->SetupTransparencyPass(m_depthTargetIndex, resourceFactory);

			m_transparencyExt->SetupTransparencyGraphicsPipelineSignatures(
				m_graphicsPipelineManager, s_renderDepthFormat
			);

			m_transparencyExt->SetCompositePass(m_postProcessingPass, resourceFactory);

			m_transparencyExt->SetupCompositePassPipeline(
				m_postProcessingPass, m_graphicsPipelineManager,
				m_renderTargetQuadModelBundle, s_renderTargetQuadMeshIndex,
				renderer
			);
		}

		// This must be done at the end so all the other passes using the render target quad
		// modelBundle can add their instance of the quad model with their own pipeline.
		// And we also must add the model bundle to the renderer before adding them to the render
		// passes.
		if (m_renderTargetQuadModelBundle.GetModelCount())
			m_renderTargetQuadModelBundleIndex = renderer.AddModelBundle(
				m_renderTargetQuadModelBundle.GetModelBundle()
			);

		// This must be done at the end so all the other passes using the post processing pass
		// can add their pipelines before we add the model bundle to the pass.
		if (hasPostProcessingPass)
			AddRenderTargetQuadToPostProcessing(renderer);

		swapchainPass->SetSwapchainCopySource(
			swapchainCopySourceIndex,
			renderer.GetExternalResourceManager().GetResourceFactory()
		);
	}

	// The render area might be a bit
	// smaller because of the title or something else, so we need to grab the render area from
	// the renderer.
	template<class Renderer_t>
	void Resize(
		this RenderPassManager& self, Renderer_t& renderer, CameraManager& cameraManager
	) {
		RendererType::Extent renderArea = renderer.GetCurrentRenderingExtent();

		self.ResizeCameras(renderArea.width, renderArea.height, cameraManager);

		self.m_mainRenderTarget.Create(
			renderArea.width, renderArea.height, renderer.GetSwapchainFormat(),
			ExternalTexture2DType::RenderTarget, ExternalTextureCreationFlags{ .copySrc = true }
		);

		self.m_depthTarget.Create(
			renderArea.width, renderArea.height, s_renderDepthFormat,
			ExternalTexture2DType::Depth
		);

		auto& resourceFactory = renderer.GetExternalResourceManager().GetResourceFactory();

		self.m_mainPass.ResetAttachmentReferences(resourceFactory);

		if (self.m_transparencyExt)
			self.m_transparencyExt->ResizeRenderTargets(
				renderArea.width, renderArea.height, renderer
			);

		if (self.m_postProcessingPass.HasImpl())
			self.m_postProcessingPass.ResetAttachmentReferences(resourceFactory);
	}

	template<class Renderer_t>
	void UpdateCameras(
		size_t frameIndex, CameraManager& cameraManager, Renderer_t& renderer
	) {
		PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(
			m_mainPassCameraIndex
		);

		camera.CalculateViewMatrix();

		renderer.UpdateCamera(frameIndex, camera.GetCamera());
	}

	[[nodiscard]]
	const GraphicsPipelineManager& GetGraphicsPipelineManager() const noexcept
	{
		return m_graphicsPipelineManager;
	}

	[[nodiscard]]
	std::uint32_t GetQuadMeshBundleIndex() const noexcept { return m_quadMeshBundleIndex; }

	[[nodiscard]]
	std::uint32_t GetMainPassCameraIndex() const noexcept { return m_mainPassCameraIndex; }

private:
	static constexpr ExternalFormat s_renderDepthFormat = ExternalFormat::D32_FLOAT;

private:
	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t SetupMainDrawingPass(Renderer_t& renderer)
	{
		auto& resourceFactory = renderer.GetExternalResourceManager().GetResourceFactory();

		const std::uint32_t renderTargetIndex = m_mainPass.AddRenderTarget(
			m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store, resourceFactory
		);

		m_mainPass.SetRenderTargetClearColour(
			renderTargetIndex, DirectX::XMFLOAT4{ 0.005f, 0.005f, 0.005f, 1.f },
			resourceFactory
		);

		m_mainPass.SetDepthTesting(
			m_depthTargetIndex, ExternalAttachmentLoadOp::Clear,
			ExternalAttachmentStoreOp::Store, resourceFactory
		);

		m_mainPass.SetDepthClearColour(1.f, resourceFactory);

		SetupMainPassSignatures(renderer);

		return renderTargetIndex;
	}

	template<class Renderer_t>
	[[nodiscard]]
	std::uint32_t SetupPostProcessingPass(Renderer_t& renderer)
	{
		const std::uint32_t renderTargetIndex = m_postProcessingPass.AddRenderTarget(
			m_mainRenderTargetIndex, ExternalAttachmentLoadOp::Load,
			ExternalAttachmentStoreOp::Store,
			renderer.GetExternalResourceManager().GetResourceFactory()
		);

		SetupPostProcessingSignatures(renderer);

		return renderTargetIndex;
	}

	template<class Renderer_t>
	void SetupRenderTargetQuad(
		Renderer_t& renderer, std::shared_ptr<ModelContainer> modelContainer
	) {
		// For some passes we would just want to combine or do some operation on an already
		// rendered texture. So, we wouldn't want to draw anything but we still need to draw
		// something to invoke the fragment shaders. So, we will bind the necessary render
		// targets as textures and a quad with the same extent as the render targets and the
		// fragment shaders of that would combine or do some other stuff.
		Mesh mesh{};

		RenderTargetQuadMesh{}.SetMesh(mesh);

		MeshBundleTempCustom quadMesh{};

		quadMesh.AddMesh(std::move(mesh));

		m_quadMeshBundleIndex = renderer.AddMeshBundle(
			quadMesh.GenerateTemporaryData(m_graphicsPipelineManager.IsMeshShaderPipeline())
		);

		m_renderTargetQuadModelBundle.SetModelContainer(std::move(modelContainer));

		m_renderTargetQuadModelBundle.SetMeshBundleIndex(m_quadMeshBundleIndex);
	}

	void SetupCameras(CameraManager& cameraManager)
	{
		m_mainPassCameraIndex = cameraManager.AddPerspectiveCamera();

		PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(m_mainPassCameraIndex);

		camera.SetProjectionMatrix(1920u, 1080u);
		camera.SetCameraPosition(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f });
	}

	void ResizeCameras(
		std::uint32_t width, std::uint32_t height, CameraManager& cameraManager
	) noexcept {
		PerspectiveCameraEuler& camera = cameraManager.GetPerspectiveCamera(m_mainPassCameraIndex);

		camera.SetProjectionMatrix(width, height);
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

	template<class Renderer_t>
	void AddRenderTargetQuadToPostProcessing(Renderer_t& renderer)
	{
		renderer.AddLocalPipelinesInExternalRenderPass(
			m_renderTargetQuadModelBundleIndex, m_postProcessingPassIndex
		);
	}

private:
	ExternalRenderPass_t    m_mainPass;
	ExternalRenderPass_t    m_postProcessingPass;
	std::uint32_t           m_mainPassIndex;
	std::uint32_t           m_postProcessingPassIndex;

	ExternalTexture_t       m_mainRenderTarget;
	ExternalTexture_t       m_depthTarget;

	std::uint32_t           m_mainPassCameraIndex;

	std::uint32_t           m_mainRenderTargetIndex;
	std::uint32_t           m_depthTargetIndex;

	std::uint32_t           m_quadMeshBundleIndex;

	std::uint32_t           m_renderTargetQuadModelBundleIndex;
	ModelBundleBase         m_renderTargetQuadModelBundle;

	TransparencyExt_t       m_transparencyExt;

	GraphicsPipelineManager m_graphicsPipelineManager;

	static constexpr std::uint32_t s_renderTargetQuadMeshIndex = 0u;

public:
	RenderPassManager(const RenderPassManager&) = delete;
	RenderPassManager& operator=(const RenderPassManager&) = delete;

	RenderPassManager(RenderPassManager&& other) noexcept
		: m_mainPass{ std::move(other.m_mainPass) },
		m_postProcessingPass{ std::move(other.m_postProcessingPass) },
		m_mainPassIndex{ other.m_mainPassIndex },
		m_postProcessingPassIndex{ other.m_postProcessingPassIndex },
		m_mainRenderTarget{ std::move(other.m_mainRenderTarget) },
		m_depthTarget{ std::move(other.m_depthTarget) },
		m_mainPassCameraIndex{ other.m_mainPassCameraIndex },
		m_mainRenderTargetIndex{ other.m_mainRenderTargetIndex },
		m_depthTargetIndex{ other.m_depthTargetIndex },
		m_quadMeshBundleIndex{ other.m_quadMeshBundleIndex },
		m_renderTargetQuadModelBundleIndex{ other.m_renderTargetQuadModelBundleIndex },
		m_renderTargetQuadModelBundle{ std::move(other.m_renderTargetQuadModelBundle) },
		m_transparencyExt{ std::move(other.m_transparencyExt) },
		m_graphicsPipelineManager{ std::move(other.m_graphicsPipelineManager) }
	{}
	RenderPassManager& operator=(RenderPassManager&& other) noexcept
	{
		m_mainPass                         = std::move(other.m_mainPass);
		m_postProcessingPass               = std::move(other.m_postProcessingPass);
		m_mainPassIndex                    = other.m_mainPassIndex;
		m_postProcessingPassIndex          = other.m_postProcessingPassIndex;
		m_mainRenderTarget                 = std::move(other.m_mainRenderTarget);
		m_depthTarget                      = std::move(other.m_depthTarget);
		m_mainPassCameraIndex              = other.m_mainPassCameraIndex;
		m_mainRenderTargetIndex            = other.m_mainRenderTargetIndex;
		m_depthTargetIndex                 = other.m_depthTargetIndex;
		m_quadMeshBundleIndex              = other.m_quadMeshBundleIndex;
		m_renderTargetQuadModelBundleIndex = other.m_renderTargetQuadModelBundleIndex;
		m_renderTargetQuadModelBundle      = std::move(other.m_renderTargetQuadModelBundle);
		m_transparencyExt                  = std::move(other.m_transparencyExt);
		m_graphicsPipelineManager          = std::move(other.m_graphicsPipelineManager);

		return *this;
	}
};
}
#endif
