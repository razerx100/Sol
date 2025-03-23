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
#include <Renderer.hpp>

class RenderPassManager
{
	using TransparencyExt_t = std::shared_ptr<WeightedTransparencyTechnique>;

public:
	RenderPassManager()
		: m_mainPass{}, m_postProcessingPass{}, m_mainRenderTarget{}, m_depthTarget{},
		m_mainRenderTargetIndex{ 0u }, m_depthTargetIndex{ 0u }, m_quadMeshBundleIndex{ 0u },
		m_renderTargetQuadModelBundleIndex{ 0u }, m_renderTargetQuadModelBundle{},
		m_transparencyExt{}, m_graphicsPipelineManager{}, m_renderer{ nullptr },
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

	void SetRenderer(Renderer* renderer) noexcept;

	void CreateResources();

	void SetupRenderPassesFromRenderer();

	void SetPostCreationReferences();

	static void ResizeCallback(void* callbackData, void* extraData);

	// The render area might be a bit
	// smaller because of the title or something else, so we need to grab the render area from the renderer.
	void Resize();

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
	[[nodiscard]]
	std::uint32_t SetupMainDrawingPass();

	[[nodiscard]]
	std::uint32_t SetupPostProcessingPass();

	void SetupRenderTargetQuad();

	void SetupMainPassSignatures();
	void SetupPostProcessingSignatures();

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

	Renderer*                           m_renderer;
	ExternalResourceFactory*            m_resourceFactory;

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
		m_renderer{ std::exchange(other.m_renderer, nullptr) },
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
		m_renderer                         = std::exchange(other.m_renderer, nullptr);
		m_resourceFactory                  = std::exchange(other.m_resourceFactory, nullptr);

		return *this;
	}
};
#endif
