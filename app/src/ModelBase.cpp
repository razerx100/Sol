#include <ranges>
#include <algorithm>
#include <cmath>

#include <ModelBase.hpp>

// ModelBase
ModelBase::ModelBase() noexcept
	: m_lightSource{ false }
{}

void ModelBase::SetAsLightSource() noexcept {
	m_lightSource = true;
}

void ModelBase::SetResources() {}

void ModelBase::PhysicsUpdate() noexcept {}

DirectX::XMMATRIX ModelBase::GetModelMatrix() const noexcept {
	return m_transform.GetModelMatrix();
}

DirectX::XMFLOAT3 ModelBase::GetModelOffset() const noexcept {
	return m_transform.GetModelOffset();
}

ModelTransform& ModelBase::GetTransform() noexcept {
	return m_transform;
}

bool ModelBase::IsLightSource() const noexcept {
	return m_lightSource;
}

// Model Bounding Box
/*ModelBoundingBox::ModelBoundingBox() noexcept : m_boundingCube{} {}

void ModelBoundingBox::Calculate(const std::vector<Vertex>& vertices) noexcept {
	DirectX::XMFLOAT3& maxPositiveAxes = m_boundingCube.positiveAxes;
	DirectX::XMFLOAT3& maxNegativeAxes = m_boundingCube.negativeAxes;

	for (const auto& vertex : vertices) {
		maxPositiveAxes.x = std::max(vertex.position.x, maxPositiveAxes.x);
		maxPositiveAxes.y = std::max(vertex.position.y, maxPositiveAxes.y);
		maxPositiveAxes.z = std::max(vertex.position.z, maxPositiveAxes.z);

		maxNegativeAxes.x = std::min(vertex.position.x, maxNegativeAxes.x);
		maxNegativeAxes.y = std::min(vertex.position.y, maxNegativeAxes.y);
		maxNegativeAxes.z = std::min(vertex.position.z, maxNegativeAxes.z);
	}
}

void ModelBoundingBox::SetBoundingCube(const ModelBounds& bounds) noexcept{
	m_boundingCube = bounds;
}

ModelBounds ModelBoundingBox::GetBoundingCube() const noexcept {
	return m_boundingCube;
}*/

// Model Transform
ModelTransform::ModelTransform() noexcept
	: m_modelMatrix{ DirectX::XMMatrixIdentity() }, m_modelOffset{ 0.f, 0.f, 0.f } {}

DirectX::XMMATRIX ModelTransform::GetModelMatrix() const noexcept {
	return m_modelMatrix;
}

DirectX::XMFLOAT3 ModelTransform::GetModelOffset() const noexcept {
	return m_modelOffset;
}

void ModelTransform::MultiplyModelMatrix(const DirectX::XMMATRIX& matrix) noexcept {
	m_modelMatrix *= matrix;
}

void ModelTransform::AddToModelOffset(const DirectX::XMFLOAT3& offset) noexcept {
	m_modelOffset.x += offset.x;
	m_modelOffset.y += offset.y;
	m_modelOffset.z += offset.z;
}

void ModelTransform::SetModelOffset(const DirectX::XMFLOAT3& offset) noexcept {
	m_modelOffset = offset;
}

void ModelTransform::Rotate(
	const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	m_modelMatrix *= DirectX::XMMatrixRotationAxis(rotationAxis, angleRadian);
}

ModelTransform& ModelTransform::RotatePitchDegree(float angle) noexcept {
	return RotatePitchRadian(DirectX::XMConvertToRadians(angle));
}

ModelTransform& ModelTransform::RotateYawDegree(float angle) noexcept {
	return RotateYawRadian(DirectX::XMConvertToRadians(angle));
}

ModelTransform& ModelTransform::RotateRollDegree(float angle) noexcept {
	return RotateRollRadian(DirectX::XMConvertToRadians(angle));
}

ModelTransform& ModelTransform::RotatePitchRadian(float angle) noexcept {
	Rotate({ 1.f, 0.f, 0.f, 0.f }, angle);

	return *this;
}

ModelTransform& ModelTransform::RotateYawRadian(float angle) noexcept {
	Rotate({ 0.f, 1.f, 0.f, 0.f }, angle);

	return *this;
}

ModelTransform& ModelTransform::RotateRollRadian(float angle) noexcept {
	Rotate({ 0.f, 0.f, 1.f, 0.f }, angle);

	return *this;
}

ModelTransform& ModelTransform::MoveTowardsX(float delta) noexcept {
	m_modelOffset.x += delta;

	return *this;
}

ModelTransform& ModelTransform::MoveTowardsY(float delta) noexcept {
	m_modelOffset.y += delta;

	return *this;
}

ModelTransform& ModelTransform::MoveTowardsZ(float delta) noexcept {
	m_modelOffset.z += delta;

	return *this;
}
