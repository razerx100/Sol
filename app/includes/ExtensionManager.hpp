#ifndef EXTENSION_MANAGER_HPP_
#define EXTENSION_MANAGER_HPP_
#include <vector>
#include <memory>
#include <BlinnPhongLightTechnique.hpp>
#include <Renderer.hpp>

class ExtensionManager
{
public:
	ExtensionManager() : m_blinnPhongLight{}, m_blinnPhongLightIndex{ 0u } {}

	void SetBlinnPhongLight(Renderer* renderer, std::uint32_t frameCount);

	void SetBuffers(Renderer* renderer);
	void SetAllExtensions(Renderer* renderer);

	void SetFixedDescriptors();

	[[nodiscard]]
	BlinnPhongLightTechnique* GetBlinnPhongLight() const noexcept { return m_blinnPhongLight.get(); }
	[[nodiscard]]
	std::shared_ptr<BlinnPhongLightTechnique> GetBlinnPhongLightSP() const noexcept
	{
		return m_blinnPhongLight;
	}

private:
	std::shared_ptr<BlinnPhongLightTechnique> m_blinnPhongLight;
	std::uint32_t                             m_blinnPhongLightIndex;

public:
	ExtensionManager(const ExtensionManager&) = delete;
	ExtensionManager& operator=(const ExtensionManager&) = delete;

	ExtensionManager(ExtensionManager&& other) noexcept
		: m_blinnPhongLight{ std::move(other.m_blinnPhongLight) },
		m_blinnPhongLightIndex{ other.m_blinnPhongLightIndex }
	{}
	ExtensionManager& operator=(ExtensionManager&& other) noexcept
	{
		m_blinnPhongLight      = std::move(other.m_blinnPhongLight);
		m_blinnPhongLightIndex = other.m_blinnPhongLightIndex;

		return *this;
	}
};
#endif
