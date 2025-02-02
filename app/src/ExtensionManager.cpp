#include <ExtensionManager.hpp>

void ExtensionManager::SetBlinnPhongLight(Renderer* renderer, std::uint32_t frameCount)
{
	m_blinnPhongLight = std::make_shared<BlinnPhongLightTechnique>(renderer, frameCount);
}

void ExtensionManager::SetBuffers(Renderer* renderer)
{
	ExternalResourceManager* resourceManager = renderer->GetExternalResourceManager();
	ExternalResourceFactory* resourceFactory = resourceManager->GetResourceFactory();

	if (m_blinnPhongLight)
		m_blinnPhongLight->SetBuffers(resourceFactory);
}

void ExtensionManager::SetAllExtensions(Renderer* renderer)
{
	ExternalResourceManager* resourceManager = renderer->GetExternalResourceManager();

	if (m_blinnPhongLight)
		m_blinnPhongLightIndex = resourceManager->AddGraphicsTechniqueExtension(m_blinnPhongLight);
}

void ExtensionManager::SetFixedDescriptors()
{
	if (m_blinnPhongLight)
		m_blinnPhongLight->SetFixedDescriptors();
}
