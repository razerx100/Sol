#include <MaterialBase.hpp>

MaterialBase::MaterialBase()
	: Material{},
	m_materialData{
		.ambient  = { 1.f, 1.f, 1.f, 1.f },
		.diffuse  = { 1.f, 1.f, 1.f, 1.f },
		.specular = { 1.f, 1.f, 1.f, 1.f }
	},
	m_diffuseIndex{ 0u }, m_specularIndex{ 0u },
	m_diffuseUVInfo{ 0.f, 0.f, 1.f, 1.f }, m_specularUVInfo{ 0.f, 0.f, 1.f, 1.f }
{}

