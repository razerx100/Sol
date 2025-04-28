#include <RenderPassManager.hpp>

namespace Sol
{
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
