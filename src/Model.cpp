#include <Model.hpp>
#include <ranges>
#include <algorithm>

ModelInputs::ModelInputs(const std::string& name) noexcept
	: m_name{ name } {}

void ModelInputs::InitData() noexcept {}

const std::vector<Vertex>& ModelInputs::GetVertexData() const noexcept {
	return m_vertices;
}

size_t ModelInputs::GetVertexBufferSize() const noexcept {
	return sizeof(Vertex) * std::size(m_vertices);
}

const std::vector<std::uint32_t>& ModelInputs::GetIndexData() const noexcept {
	return m_indices;
}

size_t ModelInputs::GetIndexBufferSize() const noexcept {
	return sizeof(std::uint32_t) * std::size(m_indices);
}

std::uint32_t ModelInputs::GetIndexCount() const noexcept {
	return static_cast<std::uint32_t>(std::size(m_indices));
}

// Model
Model::Model() noexcept
	: m_textureIndex{ 0u }, m_uvInfo{}, m_indexCount{ 0u }, m_indexOffset{ 0u },
	m_modelVelocityDirection{ 0.f, 0.f, 0.f }, m_modelVelocity{ 0.f },
	m_modelAcceleration{ 0.f } {}

std::uint32_t Model::GetIndexCount() const noexcept {
	return m_indexCount;
}

DirectX::XMMATRIX Model::GetModelMatrix() const noexcept {
	return m_transform.GetModelMatrix();
}

void Model::SetTextureIndex(size_t index) noexcept {
	m_textureIndex = static_cast<std::uint32_t>(index);
}

void Model::SetBoundingBox(const ModelBoundingBox& boundingBox) noexcept {
	m_boundingBox = boundingBox;
}

std::uint32_t Model::GetTextureIndex() const noexcept {
	return m_textureIndex;
}

void Model::SetUVInfo(float uOffset, float vOffset, float uRatio, float vRatio) noexcept {
	m_uvInfo.uOffset = uOffset;
	m_uvInfo.vOffset = vOffset;
	m_uvInfo.uRatio = uRatio;
	m_uvInfo.vRatio = vRatio;
}

void Model::SetUVInfo(const UVInfo& uvInfo) noexcept {
	m_uvInfo = uvInfo;
}

UVInfo Model::GetUVInfo() const noexcept {
	return m_uvInfo;
}

void Model::SetIndexOffset(std::uint32_t indexOffset) noexcept {
	m_indexOffset = indexOffset;
}

void Model::SetIndexCount(std::uint32_t indexCount) noexcept {
	m_indexCount = indexCount;
}

void Model::SetVelocityDirection(const DirectX::XMFLOAT3& direction) noexcept {
	m_modelVelocityDirection = direction;
}

void Model::SetVelocity(float velocity) noexcept {
	m_modelVelocity = velocity;
}

void Model::SetAcceleration(float acceleration) noexcept {
	m_modelAcceleration = acceleration;
}

void Model::SetResources() {}

void Model::PhysicsUpdate() noexcept {
	_physicsUpdate();

	DirectX::XMFLOAT3 offset{ 0.f, 0.f, 0.f };
	m_modelVelocity += m_modelAcceleration;

	offset.x = m_modelVelocityDirection.x * m_modelVelocity;
	offset.y = m_modelVelocityDirection.y * m_modelVelocity;
	offset.z = m_modelVelocityDirection.z * m_modelVelocity;

	m_transform.AddToModelOffset(offset);
}

void Model::_physicsUpdate() noexcept {}

std::uint32_t Model::GetIndexOffset() const noexcept {
	return m_indexOffset;
}

ModelBounds Model::GetBoundingBox() const noexcept {
	return m_boundingBox.GetBoundingCube();
}

DirectX::XMFLOAT3 Model::GetModelOffset() const noexcept {
	return m_transform.GetModelOffset();
}

ModelTransform& Model::GetTransform() noexcept {
	return m_transform;
}

ModelBoundingBox& Model::GetBoundingBox() noexcept {
	return m_boundingBox;
}

// Model Bounding Box
ModelBoundingBox::ModelBoundingBox() noexcept : m_boundingCube{} {}

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
}

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

void ModelTransform::Rotate(
	const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	m_modelMatrix *= DirectX::XMMatrixRotationAxis(rotationAxis, angleRadian);
}

ModelTransform& ModelTransform::RotateXDegree(float angle) noexcept {
	Rotate({ 1.f, 0.f, 0.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

ModelTransform& ModelTransform::RotateYDegree(float angle) noexcept {
	Rotate({ 0.f, 1.f, 0.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

ModelTransform& ModelTransform::RotateZDegree(float angle) noexcept {
	Rotate({ 0.f, 0.f, 1.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

ModelTransform& ModelTransform::RotateXRadian(float angle) noexcept {
	Rotate({ 1.f, 0.f, 0.f, 0.f }, angle);

	return *this;
}

ModelTransform& ModelTransform::RotateYRadian(float angle) noexcept {
	Rotate({ 0.f, 1.f, 0.f, 0.f }, angle);

	return *this;
}

ModelTransform& ModelTransform::RotateZRadian(float angle) noexcept {
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
