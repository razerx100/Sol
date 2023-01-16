#ifndef BASIC_MODELS_HPP_
#define BASIC_MODELS_HPP_
#include <Model.hpp>

class ModelWithVelocity : public Model {
public:
	ModelWithVelocity() noexcept;

	void SetVelocityDirection(const DirectX::XMFLOAT3& direction) noexcept;
	void SetVelocitySpeed(float velocitySpeed) noexcept;
	void SetAcceleration(float acceleration) noexcept;

	void PhysicsUpdate() noexcept final;

protected:
	virtual void _physicsUpdate() noexcept;

protected:
	DirectX::XMFLOAT3 m_modelVelocityDirection;
	float m_modelVelocitySpeed;
	float m_modelAcceleration;
};

class ScalableModel : public Model {
public:
	ScalableModel(float scale) noexcept;

	virtual void SetResources() override;

	void SetTextureFromAtlas(const std::string& texName) noexcept;

private:
	std::string m_subTextureName;
};

// Revolving around 0, 0, 0
class OrbitingModel : public ScalableModel {
public:
	OrbitingModel(float scale) noexcept;

	void MeasureRadius() noexcept;

	void PhysicsUpdate() noexcept final;

protected:
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetAngularOffset() noexcept = 0;

protected:
	float m_angle;
	float m_radius;
	float m_modelDirectionX;
	float m_modelDirectionY;
};

class OrbitModelClock : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetAngularOffset() noexcept final;
};

class OrbitModelAntiClock : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetAngularOffset() noexcept final;
};
#endif
