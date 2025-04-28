#ifndef EXTENSION_MANAGER_HPP_
#define EXTENSION_MANAGER_HPP_
#include <vector>
#include <memory>
#include <BlinnPhongLightTechnique.hpp>
#include <WeightedTransparencyTechnique.hpp>

namespace Sol
{
class ExtensionManager
{
public:
	ExtensionManager()
		: m_blinnPhongLight{}, m_weightedTransparency{}, m_blinnPhongLightIndex { 0u },
		m_weightedTransparencyIndex{ 0u }
	{}

	void SetBlinnPhongLight(std::uint32_t frameCount);
	void SetWeightedTransparency();

	template<class Renderer_t>
	void SetBuffers(Renderer_t& renderer)
	{
		auto& resourceManager = renderer.GetExternalResourceManager();
		auto& resourceFactory = resourceManager.GetResourceFactory();

		if (m_blinnPhongLight)
			m_blinnPhongLight->SetBuffers(resourceFactory);

		if (m_weightedTransparency)
			m_weightedTransparency->SetBuffers(resourceFactory);
	}

	template<class Renderer_t>
	void SetAllExtensions(Renderer_t& renderer)
	{
		auto& resourceManager = renderer.GetExternalResourceManager();

		if (m_blinnPhongLight)
			m_blinnPhongLightIndex = resourceManager.AddGraphicsTechniqueExtension(
				m_blinnPhongLight
			);

		if (m_weightedTransparency)
			m_weightedTransparencyIndex = resourceManager.AddGraphicsTechniqueExtension(
				m_weightedTransparency
			);
	}

	template<class Renderer_t>
	void SetFixedDescriptors(Renderer_t& renderer)
	{
		if (m_blinnPhongLight)
			m_blinnPhongLight->SetFixedDescriptors(renderer);

		if (m_weightedTransparency)
			m_weightedTransparency->SetFixedDescriptors(renderer);
	}

	[[nodiscard]]
	WeightedTransparencyTechnique* GetWeightedTransparency() const noexcept
	{
		return m_weightedTransparency.get();
	}
	[[nodiscard]]
	std::shared_ptr<WeightedTransparencyTechnique> GetWeightedTransparencySP() const noexcept
	{
		return m_weightedTransparency;
	}

	[[nodiscard]]
	BlinnPhongLightTechnique* GetBlinnPhongLight() const noexcept { return m_blinnPhongLight.get(); }
	[[nodiscard]]
	std::shared_ptr<BlinnPhongLightTechnique> GetBlinnPhongLightSP() const noexcept
	{
		return m_blinnPhongLight;
	}

private:
	std::shared_ptr<BlinnPhongLightTechnique>      m_blinnPhongLight;
	std::shared_ptr<WeightedTransparencyTechnique> m_weightedTransparency;
	// Could use the indices to remove the extensions.
	std::uint32_t                                  m_blinnPhongLightIndex;
	std::uint32_t                                  m_weightedTransparencyIndex;

public:
	ExtensionManager(const ExtensionManager&) = delete;
	ExtensionManager& operator=(const ExtensionManager&) = delete;

	ExtensionManager(ExtensionManager&& other) noexcept
		: m_blinnPhongLight{ std::move(other.m_blinnPhongLight) },
		m_weightedTransparency{ std::move(other.m_weightedTransparency) },
		m_blinnPhongLightIndex{ other.m_blinnPhongLightIndex },
		m_weightedTransparencyIndex{ other.m_weightedTransparencyIndex }
	{}
	ExtensionManager& operator=(ExtensionManager&& other) noexcept
	{
		m_blinnPhongLight           = std::move(other.m_blinnPhongLight);
		m_weightedTransparency      = std::move(other.m_weightedTransparency);
		m_blinnPhongLightIndex      = other.m_blinnPhongLightIndex;
		m_weightedTransparencyIndex = other.m_weightedTransparencyIndex;

		return *this;
	}
};
}
#endif
