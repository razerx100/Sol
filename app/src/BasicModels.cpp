#include <cmath>

#include <BasicModels.hpp>
#include <DirectXMath.h>
#include <Sol.hpp>

// Velocity
ModelWithVelocity::ModelWithVelocity() noexcept
	: m_modelVelocityDirection{ 0.f, 0.f, 0.f }, m_modelVelocitySpeed{ 0.f },
	m_modelAcceleration{ 0.f } {}

void ModelWithVelocity::SetVelocityDirection(const DirectX::XMFLOAT3& direction) noexcept {
	m_modelVelocityDirection = direction;
}

void ModelWithVelocity::SetVelocitySpeed(float velocitySpeed) noexcept {
	m_modelVelocitySpeed = velocitySpeed;
}

void ModelWithVelocity::SetAcceleration(float acceleration) noexcept {
	m_modelAcceleration = acceleration;
}

void ModelWithVelocity::PhysicsUpdate() noexcept {
	_physicsUpdate();

	m_modelVelocitySpeed += m_modelAcceleration;

	DirectX::XMFLOAT3 offset{ 0.f, 0.f, 0.f };
	offset.x = m_modelVelocityDirection.x * m_modelVelocitySpeed;
	offset.y = m_modelVelocityDirection.y * m_modelVelocitySpeed;
	offset.z = m_modelVelocityDirection.z * m_modelVelocitySpeed;

	GetTransform().AddToModelOffset(offset);
}

void ModelWithVelocity::_physicsUpdate() noexcept {}

// Scalable
ScalableModel::ScalableModel(float scale) noexcept {
	GetTransform().MultiplyModelMatrix(DirectX::XMMatrixScaling(scale, scale, scale));
}

void ScalableModel::SetTextureFromAtlas(
	const std::string& texName, TextureType type
) noexcept {
	m_textureInfo.emplace_back(texName, type);
}

void ScalableModel::SetResources() {
	for(const auto& texInfo : m_textureInfo) {
		UVInfo uvInfo = Sol::textureAtlas->GetUVInfo(texInfo.name);
		size_t texIndex = Sol::textureAtlas->GetTextureIndex();

		/*switch (texInfo.type) {
		case TextureType::diffuse: {
			SetDiffuseTexUVInfo(uvInfo);
			SetDiffuseTexIndex(texIndex);

			break;
		}
		case TextureType::specular: {
			SetSpecularTexUVInfo(uvInfo);
			SetSpecularTexIndex(texIndex);

			break;
		}
		}*/
	}
}

// Orbit
OrbitingModel::OrbitingModel(float scale) noexcept
	: ScalableModel{ scale }, m_angle{ 0.f }, m_radius{ 0.f }, m_modelDirectionX{ 0.f },
	m_modelDirectionY{ 0.f } {}

void OrbitingModel::PhysicsUpdate() noexcept {
	static constexpr float speedModifier = 0.05f;

	const DirectX::XMFLOAT3 offset = GetAngularOffset();
	GetTransform().SetModelOffset(offset);

	m_angle += speedModifier;
	if (m_angle > DirectX::XM_2PI)
		m_angle = 0.f;
}

void OrbitingModel::MeasureRadius() noexcept {
	DirectX::XMFLOAT3 modelLocation = GetTransform().GetModelOffset();
	m_radius = std::sqrt(
		std::pow(modelLocation.x, 2.f) + std::pow(modelLocation.y, 2.f) +
		std::pow(modelLocation.z, 2.f)
	);

	// Direction of X and Y decides the orbit direction
	m_modelDirectionX = modelLocation.x / m_radius;
	m_modelDirectionY = modelLocation.y / m_radius;
}

// Orbit ClockWise
DirectX::XMFLOAT3 OrbitModelClock::GetAngularOffset() noexcept {
	return {
		m_modelDirectionX * std::sin(m_angle) * m_radius,
		m_modelDirectionY * std::sin(m_angle) * m_radius,
		std::cos(m_angle) * m_radius
	};
}

// Orbit Anti-Clockwise
DirectX::XMFLOAT3 OrbitModelAntiClock::GetAngularOffset() noexcept {
	return {
		m_modelDirectionX * std::cos(m_angle) * m_radius,
		m_modelDirectionY * std::cos(m_angle) * m_radius,
		std::sin(m_angle) * m_radius
	};
}
