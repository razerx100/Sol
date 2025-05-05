#ifndef EXTENSION_MANAGER_HPP_
#define EXTENSION_MANAGER_HPP_
#include <vector>
#include <memory>
#include <BlinnPhongLightTechnique.hpp>
#include <WeightedTransparencyTechnique.hpp>

namespace Sol
{
template<class ExternalRenderPassImpl_t, class ExternalBufferImpl_t, class ExternalTextureImpl_t>
class ExtensionManager
{
public:
	using WeightedTransparency_t = WeightedTransparencyTechnique<
		ExternalRenderPassImpl_t, ExternalBufferImpl_t, ExternalTextureImpl_t
	>;
	using TransparencyExt_t      = std::shared_ptr<WeightedTransparency_t>;

	using BlinnPhongLightTechnique_t = BlinnPhongLightTechnique<ExternalBufferImpl_t>;
	using BlinnPhongLightExt_t       = std::shared_ptr<BlinnPhongLightTechnique_t>;

public:
	ExtensionManager()
		: m_blinnPhongLight{}, m_weightedTransparency{}, m_blinnPhongLightIndex { 0u },
		m_weightedTransparencyIndex{ 0u }
	{}

	void SetBlinnPhongLight(std::uint32_t frameCount)
	{
		m_blinnPhongLight = std::make_shared<BlinnPhongLightTechnique_t>(frameCount);
	}

	void SetWeightedTransparency()
	{
		m_weightedTransparency = std::make_shared<WeightedTransparency_t>();
	}

	void UpdateCPUData(size_t frameIndex) noexcept
	{
		if (m_blinnPhongLight)
			m_blinnPhongLight->UpdateCPUData(frameIndex);
	}

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
				m_blinnPhongLight->GetGfxTechniqueData()
			);

		if (m_weightedTransparency)
			m_weightedTransparencyIndex = resourceManager.AddGraphicsTechniqueExtension(
				m_weightedTransparency->GetGfxTechniqueData()
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
	WeightedTransparency_t* GetWeightedTransparency() const noexcept
	{
		return m_weightedTransparency.get();
	}
	[[nodiscard]]
	TransparencyExt_t GetWeightedTransparencySP() const noexcept
	{
		return m_weightedTransparency;
	}

	[[nodiscard]]
	BlinnPhongLightTechnique_t* GetBlinnPhongLight() const noexcept
	{
		return m_blinnPhongLight.get();
	}
	[[nodiscard]]
	BlinnPhongLightExt_t GetBlinnPhongLightSP() const noexcept
	{
		return m_blinnPhongLight;
	}

private:
	BlinnPhongLightExt_t m_blinnPhongLight;
	TransparencyExt_t    m_weightedTransparency;
	// Could use the indices to remove the extensions.
	std::uint32_t        m_blinnPhongLightIndex;
	std::uint32_t        m_weightedTransparencyIndex;

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
