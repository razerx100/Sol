#ifndef GRAPHICS_PIPELINE_MANAGER_HPP_
#define GRAPHICS_PIPELINE_MANAGER_HPP_
#include <ExternalPipeline.hpp>

class GraphicsPipelineManager
{
public:
	GraphicsPipelineManager();

	void AddRenderTarget(ExternalFormat format);
	void SetDepthTarget(ExternalFormat format);
	void SetStencilTarget(ExternalFormat format);

	[[nodiscard]]
	const ExternalGraphicsPipeline& GetAlphaBlendingSignature() const noexcept
	{
		return m_alphaBlendingSignature;
	}

private:
	void SetupAlphaBlending() noexcept;

private:
	ExternalGraphicsPipeline m_alphaBlendingSignature;

public:
	GraphicsPipelineManager(const GraphicsPipelineManager&) = delete;
	GraphicsPipelineManager& operator=(const GraphicsPipelineManager&) = delete;

	GraphicsPipelineManager(GraphicsPipelineManager&& other) noexcept
		: m_alphaBlendingSignature{ std::move(other.m_alphaBlendingSignature) }
	{}
	GraphicsPipelineManager& operator=(GraphicsPipelineManager&& other) noexcept
	{
		m_alphaBlendingSignature = std::move(other.m_alphaBlendingSignature);

		return *this;
	}
};
#endif
