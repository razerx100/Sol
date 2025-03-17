#include <GraphicsPipelineManager.hpp>

GraphicsPipelineManager::GraphicsPipelineManager()
	: m_nonAlphaClippingSignature{}
{
	SetupNonAlphaClipping();
}

void GraphicsPipelineManager::SetupNonAlphaClipping() noexcept
{
	m_nonAlphaClippingSignature.EnableBackfaceCulling();
}

void GraphicsPipelineManager::AddRenderTarget(ExternalFormat format)
{
	m_nonAlphaClippingSignature.AddRenderTarget(format, ExternalBlendState{});
}

void GraphicsPipelineManager::SetDepthTarget(ExternalFormat format)
{
	m_nonAlphaClippingSignature.EnableDepthTesting(format);
}

void GraphicsPipelineManager::SetStencilTarget(ExternalFormat format)
{
	m_nonAlphaClippingSignature.EnableStencilTesting(format);
}
