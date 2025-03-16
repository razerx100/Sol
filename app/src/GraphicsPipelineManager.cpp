#include <GraphicsPipelineManager.hpp>

GraphicsPipelineManager::GraphicsPipelineManager()
	: m_alphaBlendingSignature{}
{
	SetupAlphaBlending();
}

void GraphicsPipelineManager::SetupAlphaBlending() noexcept
{
	m_alphaBlendingSignature.EnableBackfaceCulling();
}

void GraphicsPipelineManager::AddRenderTarget(ExternalFormat format)
{
	m_alphaBlendingSignature.AddRenderTarget(format);
}

void GraphicsPipelineManager::SetDepthTarget(ExternalFormat format)
{
	m_alphaBlendingSignature.EnableDepthTesting(format);
}

void GraphicsPipelineManager::SetStencilTarget(ExternalFormat format)
{
	m_alphaBlendingSignature.EnableStencilTesting(format);
}
