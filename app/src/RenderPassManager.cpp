#include <RenderPassManager.hpp>

namespace Sol
{
void RenderPassManager::CreateResources()
{
	m_mainRenderTargetIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_mainRenderTarget      = m_resourceFactory->GetExternalTextureSP(m_mainRenderTargetIndex);

	m_depthTargetIndex = static_cast<std::uint32_t>(m_resourceFactory->CreateExternalTexture());

	m_depthTarget      = m_resourceFactory->GetExternalTextureSP(m_depthTargetIndex);
}

void RenderPassManager::AddRenderTargetQuadToPostProcessing()
{
	m_postProcessingPass->AddModelBundle(m_renderTargetQuadModelBundleIndex);
}

void RenderPassManager::SetTransparencyPass(
	std::shared_ptr<WeightedTransparencyTechnique> transparencyExt
) {
	m_transparencyExt = std::move(transparencyExt);
}
}
