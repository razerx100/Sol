#ifndef MATERIAL_BASE_HPP_
#define MATERIAL_BASE_HPP_
#include <Material.hpp>

class MaterialBase : public Material
{
public:
	MaterialBase()
		: Material{},
		m_materialData{
			.ambient  = { 0.f, 0.f, 0.f, 1.f },
			.diffuse  = { 1.f, 1.f, 1.f, 1.f },
			.specular = { 0.f, 0.f, 0.f, 1.f }
		}
	{}
	MaterialBase(const MaterialData& materialData) : Material{}, m_materialData{ materialData } {}

	void SetData(const MaterialData& data) noexcept { m_materialData = data; }

	[[nodiscard]]
	MaterialData Get() const noexcept override { return m_materialData; }

private:
	MaterialData  m_materialData;
};
#endif
