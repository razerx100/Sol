#ifndef EXTENSION_MANAGER_HPP_
#define EXTENSION_MANAGER_HPP_
#include <vector>
#include <memory>
#include <BlinnPhongLightTechnique.hpp>
#include <WeightedTransparencyTechnique.hpp>
#include <Renderer.hpp>

class ExtensionManager
{
public:
	ExtensionManager()
		: m_blinnPhongLight{}, m_weightedTransparency{}, m_blinnPhongLightIndex { 0u },
		m_weightedTransparencyIndex{ 0u }
	{}

	void SetBlinnPhongLight(Renderer* renderer, std::uint32_t frameCount);
	void SetWeightedTransparency(Renderer* renderer);

	void SetBuffers(Renderer* renderer);
	void SetAllExtensions(Renderer* renderer);

	void SetFixedDescriptors();

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
#endif
