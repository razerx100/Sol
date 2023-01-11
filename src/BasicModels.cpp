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

void ScalableModel::SetTextureName(const std::string& texName) noexcept {
	m_textureName = texName;
}

void ScalableModel::SetResources() {
	UVInfo uvInfo = Sol::textureAtlas->GetUVInfo(m_textureName);

	SetUVInfo(uvInfo);
}

// Orbit
OrbitingModel::OrbitingModel(float scale) noexcept
	: ScalableModel{ scale }, m_angle{ 0.f }, m_radius{ 0.f } {}

void OrbitingModel::PhysicsUpdate() noexcept {
	static constexpr float speedModifier = 0.05f;

	DirectX::XMFLOAT3 offset = SetAngularVelocity();
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
}

DirectX::XMFLOAT3 OrbitingModel::SetAngularVelocity() noexcept {
	return GetTransform().GetModelOffset();
}

// Orbit XC
DirectX::XMFLOAT3 OrbitModelXC::SetAngularVelocity() noexcept {
	return {
		std::sin(m_angle) * m_radius,
		0.f,
		std::cos(m_angle) * m_radius
	};
}

// Orbit XAC
DirectX::XMFLOAT3 OrbitModelXAC::SetAngularVelocity() noexcept {
	return {
		std::cos(m_angle) * m_radius,
		0.f,
		std::sin(m_angle) * m_radius
	};
}

// Orbit YC
DirectX::XMFLOAT3 OrbitModelYC::SetAngularVelocity() noexcept {
	return {
		0.f,
		std::sin(m_angle) * m_radius,
		std::cos(m_angle) * m_radius
	};
}

// Orbit TLC
DirectX::XMFLOAT3 OrbitModelTLC::SetAngularVelocity() noexcept {
	return {
		std::sin(m_angle) * m_radius,
		std::sin(m_angle) * m_radius,
		std::cos(m_angle) * m_radius
	};
}

// Orbit TRC
DirectX::XMFLOAT3 OrbitModelTRC::SetAngularVelocity() noexcept {
	return {
		-std::sin(m_angle) * m_radius,
		std::sin(m_angle) * m_radius,
		std::cos(m_angle) * m_radius
	};
}
