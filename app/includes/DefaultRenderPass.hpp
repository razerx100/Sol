#ifndef DEFAULT_RENDER_PASS_HPP_
#define DEFAULT_RENDER_PASS_HPP_
#include <memory>
#include <utility>
#include <ExternalBuffer.hpp>
#include <ExternalRenderPass.hpp>
#include <ExternalResourceFactory.hpp>
#include <Renderer.hpp>

class DefaultRenderPass
{
public:
	DefaultRenderPass()
		: m_renderTarget{}, m_depthBuffer{}, m_renderPass{}, m_renderer{ nullptr },
		m_renderTargetIndex{ 0u }, m_depthBufferIndex{ 0u }
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

	void SetRenderTarget(ExternalResourceFactory& resourceFactory);
	void SetDepthBuffer(ExternalResourceFactory& resourceFactory);

	void SetRenderer(Renderer* renderer) noexcept { m_renderer = renderer; }

	void FetchRenderPassFromRenderer();

	static void ResizeCallback(void* callbackData, void* extraData);

	// The render area might be a bit
	// smaller because of the title or something else, so we need to grab the render area from the renderer.
	void Resize();

	[[nodiscard]]
	std::uint32_t GetRenderTargetIndex() const noexcept { return m_renderTargetIndex; }
	[[nodiscard]]
	std::uint32_t GetDepthBufferIndex() const noexcept { return m_depthBufferIndex; }

private:
	std::shared_ptr<ExternalTexture>    m_renderTarget;
	std::shared_ptr<ExternalTexture>    m_depthBuffer;
	std::shared_ptr<ExternalRenderPass> m_renderPass;
	Renderer*                           m_renderer;
	std::uint32_t                       m_renderTargetIndex;
	std::uint32_t                       m_depthBufferIndex;

public:
	DefaultRenderPass(const DefaultRenderPass&) = delete;
	DefaultRenderPass& operator=(const DefaultRenderPass&) = delete;

	DefaultRenderPass(DefaultRenderPass&& other) noexcept
		: m_renderTarget{ std::move(other.m_renderTarget) },
		m_depthBuffer{ std::move(other.m_depthBuffer) },
		m_renderPass{ std::move(other.m_renderPass) },
		m_renderer{ std::exchange(other.m_renderer, nullptr) },
		m_renderTargetIndex{ other.m_renderTargetIndex },
		m_depthBufferIndex{ other.m_depthBufferIndex }
	{}
	DefaultRenderPass& operator=(DefaultRenderPass&& other) noexcept
	{
		m_renderTarget      = std::move(other.m_renderTarget);
		m_depthBuffer       = std::move(other.m_depthBuffer);
		m_renderPass        = std::move(other.m_renderPass);
		m_renderer          = std::exchange(other.m_renderer, nullptr);
		m_renderTargetIndex = other.m_renderTargetIndex;
		m_depthBufferIndex  = other.m_depthBufferIndex;

		return *this;
	}
};
#endif
