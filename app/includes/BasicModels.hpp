#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <ModelBase.hpp>

class ModelWithVelocity
{
public:
	ModelWithVelocity() noexcept;

	void SetVelocityDirection(const DirectX::XMFLOAT3& direction) noexcept;
	void SetVelocitySpeed(float velocitySpeed) noexcept;
	void SetAcceleration(float acceleration) noexcept;

	void PhysicsUpdate() noexcept;

protected:
	virtual void _physicsUpdate() noexcept;

protected:
	DirectX::XMFLOAT3 m_modelVelocityDirection;
	float m_modelVelocitySpeed;
	float m_modelAcceleration;
};

enum class TextureType
{
	diffuse,
	specular
};

class ScalableModel
{
public:
	ScalableModel(float scale);
};

// Revolving around 0, 0, 0
class OrbitingModel : public ScalableModel
{
public:
	OrbitingModel(float scale) noexcept;

	void MeasureRadius() noexcept;

	void PhysicsUpdate() noexcept;

protected:
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetAngularOffset() noexcept = 0;

protected:
	float m_angle;
	float m_radius;
	float m_modelDirectionX;
	float m_modelDirectionY;
};

class OrbitModelClock : public OrbitingModel
{
public:
	using OrbitingModel::OrbitingModel;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetAngularOffset() noexcept final;
};

class OrbitModelAntiClock : public OrbitingModel
{
public:
	using OrbitingModel::OrbitingModel;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetAngularOffset() noexcept final;
};
#endif
