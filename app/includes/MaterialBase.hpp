#ifndef MATERIAL_BASE_HPP_
#define MATERIAL_BASE_HPP_
#include <Material.hpp>


class MaterialBase : public Material
{
public:
	MaterialBase();

	void SetDiffuseIndex(size_t index) noexcept
	{
		m_diffuseIndex = static_cast<std::uint32_t>(index);
	}
	void SetSpecularIndex(size_t index) noexcept
	{
		m_specularIndex = static_cast<std::uint32_t>(index);
	}

	void SetDiffuseUVInfo(
		float uOffset, float vOffset, float uRatio, float vRatio
	) noexcept {
		SetDiffuseUVInfo(
			UVInfo{ .uOffset = uOffset, .vOffset = vOffset, .uRatio = uRatio, .vRatio = vRatio }
		);
	}
	void SetDiffuseUVInfo(const UVInfo& uvInfo) noexcept { m_diffuseUVInfo = uvInfo; }
	void SetSpecularUVInfo(
		float uOffset, float vOffset, float uRatio, float vRatio
	) noexcept {
		SetSpecularUVInfo(
			UVInfo{ .uOffset = uOffset, .vOffset = vOffset, .uRatio = uRatio, .vRatio = vRatio }
		);
	}
	void SetSpecularUVInfo(const UVInfo& uvInfo) noexcept { m_specularUVInfo = uvInfo; }

	void SetData(const MaterialData& data) noexcept { m_materialData = data; }

	[[nodiscard]]
	MaterialData Get() const noexcept override { return m_materialData; }
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override { return m_diffuseIndex; }
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override { return m_diffuseUVInfo; }
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override { return m_specularIndex; }
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override { return m_specularUVInfo; }

private:
	MaterialData  m_materialData;
	std::uint32_t m_diffuseIndex;
	std::uint32_t m_specularIndex;
	UVInfo        m_diffuseUVInfo;
	UVInfo        m_specularUVInfo;
};
#endif
