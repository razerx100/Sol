#include <ExtensionManager.hpp>

void ExtensionManager::SetBlinnPhongLight(std::uint32_t frameCount)
{
	m_blinnPhongLight = std::make_shared<BlinnPhongLightTechnique>(frameCount);
}

void ExtensionManager::SetWeightedTransparency()
{
	m_weightedTransparency = std::make_shared<WeightedTransparencyTechnique>();
}
