#include <ExtensionManager.hpp>

void ExtensionManager::SetBlinnPhongLight(Renderer* renderer, std::uint32_t frameCount)
{
	m_blinnPhongLight = std::make_shared<BlinnPhongLightTechnique>(renderer, frameCount);
}

void ExtensionManager::SetWeightedTransparency(Renderer* renderer)
{
	m_weightedTransparency = std::make_shared<WeightedTransparencyTechnique>(renderer);
}

void ExtensionManager::SetBuffers(Renderer* renderer)
{
	ExternalResourceManager* resourceManager = renderer->GetExternalResourceManager();
	ExternalResourceFactory* resourceFactory = resourceManager->GetResourceFactory();

	if (m_blinnPhongLight)
		m_blinnPhongLight->SetBuffers(resourceFactory);

	if (m_weightedTransparency)
		m_weightedTransparency->SetBuffers(resourceFactory);
}

void ExtensionManager::SetAllExtensions(Renderer* renderer)
{
	ExternalResourceManager* resourceManager = renderer->GetExternalResourceManager();

	if (m_blinnPhongLight)
		m_blinnPhongLightIndex = resourceManager->AddGraphicsTechniqueExtension(m_blinnPhongLight);

	if (m_weightedTransparency)
		m_weightedTransparencyIndex = resourceManager->AddGraphicsTechniqueExtension(m_weightedTransparency);
}

void ExtensionManager::SetFixedDescriptors()
{
	if (m_blinnPhongLight)
		m_blinnPhongLight->SetFixedDescriptors();

	if (m_weightedTransparency)
		m_weightedTransparency->SetFixedDescriptors();
}
