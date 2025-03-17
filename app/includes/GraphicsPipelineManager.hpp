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
	const ExternalGraphicsPipeline& GetNonAlphaClippingSignature() const noexcept
	{
		return m_nonAlphaClippingSignature;
	}

private:
	void SetupNonAlphaClipping() noexcept;

private:
	ExternalGraphicsPipeline m_nonAlphaClippingSignature;

public:
	GraphicsPipelineManager(const GraphicsPipelineManager&) = delete;
	GraphicsPipelineManager& operator=(const GraphicsPipelineManager&) = delete;

	GraphicsPipelineManager(GraphicsPipelineManager&& other) noexcept
		: m_nonAlphaClippingSignature{ std::move(other.m_nonAlphaClippingSignature) }
	{}
	GraphicsPipelineManager& operator=(GraphicsPipelineManager&& other) noexcept
	{
		m_nonAlphaClippingSignature = std::move(other.m_nonAlphaClippingSignature);

		return *this;
	}
};
#endif
