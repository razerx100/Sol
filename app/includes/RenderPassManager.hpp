#ifndef RENDER_PASS_MANAGER_HPP_
#define RENDER_PASS_MANAGER_HPP_
#include <memory>
#include <utility>
#include <ExternalBuffer.hpp>
#include <ExternalRenderPass.hpp>
#include <ExternalResourceFactory.hpp>
#include <GraphicsPipelineManager.hpp>
#include <Renderer.hpp>

class RenderPassManager
{
public:
	RenderPassManager()
		: m_renderTarget{}, m_depthTarget{}, m_renderPass{}, m_renderTargetIndex{ 0u },
		m_depthBufferIndex{ 0u }, m_renderPipelineManager{}, m_renderer{ nullptr },
		m_resourceFactory{ nullptr }
	{}

	void AddPipeline(std::uint32_t pipelineIndex)
	{
		m_renderPass->AddPipeline(pipelineIndex);
	}

	void AddModelBundle(std::uint32_t bundleIndex)
	{
		m_renderPass->AddModelBundle(bundleIndex);
	}

	void RemoveModelBundle(std::uint32_t bundleIndex) noexcept
	{
		m_renderPass->RemoveModelBundle(bundleIndex);
	}
	void RemovePipeline(std::uint32_t pipelineIndex) noexcept
	{
		m_renderPass->RemovePipeline(pipelineIndex);
	}

	void SetRenderer(Renderer* renderer) noexcept;

	void CreateAttachments();

	void SetupRenderPassesFromRenderer();

	static void ResizeCallback(void* callbackData, void* extraData);

	// The render area might be a bit
	// smaller because of the title or something else, so we need to grab the render area from the renderer.
	void Resize();

	[[nodiscard]]
	const GraphicsPipelineManager& GetRenderPipelineManager() const noexcept
	{
		return m_renderPipelineManager;
	}

private:
	static constexpr ExternalFormat s_renderDepthFormat = ExternalFormat::D32_FLOAT;

private:
	void SetupRenderPass();
	void SetupRenderPipelineManager();

private:
	std::shared_ptr<ExternalTexture>    m_renderTarget;
	std::shared_ptr<ExternalTexture>    m_depthTarget;
	std::shared_ptr<ExternalRenderPass> m_renderPass;
	std::uint32_t                       m_renderTargetIndex;
	std::uint32_t                       m_depthBufferIndex;

	GraphicsPipelineManager             m_renderPipelineManager;

	Renderer*                           m_renderer;
	ExternalResourceFactory*            m_resourceFactory;

public:
	RenderPassManager(const RenderPassManager&) = delete;
	RenderPassManager& operator=(const RenderPassManager&) = delete;

	RenderPassManager(RenderPassManager&& other) noexcept
		: m_renderTarget{ std::move(other.m_renderTarget) },
		m_depthTarget{ std::move(other.m_depthTarget) },
		m_renderPass{ std::move(other.m_renderPass) },
		m_renderTargetIndex{ other.m_renderTargetIndex },
		m_depthBufferIndex{ other.m_depthBufferIndex },
		m_renderPipelineManager{ std::move(other.m_renderPipelineManager) },
		m_renderer{ std::exchange(other.m_renderer, nullptr) },
		m_resourceFactory{ std::exchange(other.m_resourceFactory, nullptr) }
	{}
	RenderPassManager& operator=(RenderPassManager&& other) noexcept
	{
		m_renderTarget          = std::move(other.m_renderTarget);
		m_depthTarget           = std::move(other.m_depthTarget);
		m_renderPass            = std::move(other.m_renderPass);
		m_renderTargetIndex     = other.m_renderTargetIndex;
		m_depthBufferIndex      = other.m_depthBufferIndex;
		m_renderPipelineManager = std::move(other.m_renderPipelineManager);
		m_renderer              = std::exchange(other.m_renderer, nullptr);
		m_resourceFactory       = std::exchange(other.m_resourceFactory, nullptr);

		return *this;
	}
};
#endif
