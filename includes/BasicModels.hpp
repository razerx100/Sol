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

	void SetTextureName(const std::string& texName) noexcept;

private:
	std::string m_textureName;
};

class OrbitingModel : public ScalableModel {
public:
	OrbitingModel(float scale) noexcept;

	void MeasureRadius() noexcept;

	void PhysicsUpdate() noexcept final;

protected:
	virtual DirectX::XMFLOAT3 SetAngularVelocity() noexcept;

protected:
	float m_angle;
	float m_radius;
};

class OrbitModelXC : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

protected:
	DirectX::XMFLOAT3 SetAngularVelocity() noexcept final;
};

class OrbitModelXAC : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

protected:
	DirectX::XMFLOAT3 SetAngularVelocity() noexcept final;
};

class OrbitModelYC : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

protected:
	DirectX::XMFLOAT3 SetAngularVelocity() noexcept final;
};

class OrbitModelTLC : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

protected:
	DirectX::XMFLOAT3 SetAngularVelocity() noexcept final;
};

class OrbitModelTRC : public OrbitingModel {
public:
	using OrbitingModel::OrbitingModel;

protected:
	DirectX::XMFLOAT3 SetAngularVelocity() noexcept final;
};
#endif
