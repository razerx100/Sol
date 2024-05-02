#include <ranges>
#include <algorithm>
#include <cmath>

#include <ModelBase.hpp>

ModelInputs::ModelInputs(const std::string& name) noexcept : m_name{ name } {}

void ModelInputs::InitData() noexcept {}

void ModelInputs::CalculateNormalsIndependentFaces() noexcept {
	for (size_t index = 0u; index < std::size(m_indices); index += 3) {
		Vertex& vertex1 = m_vertices[m_indices[index]];
		Vertex& vertex2 = m_vertices[m_indices[index + 1u]];
		Vertex& vertex3 = m_vertices[m_indices[index + 2u]];

		DirectX::XMFLOAT3 faceNormal = GetFaceNormal(
			vertex1.position, vertex2.position, vertex3.position
		);

		vertex1.normal = faceNormal;
		vertex2.normal = faceNormal;
		vertex3.normal = faceNormal;
	}
}

void ModelInputs::SetInputName(const std::string& name) noexcept {
	m_name = name;
}

DirectX::XMFLOAT3 ModelInputs::GetFaceNormal(
	const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
	const DirectX::XMFLOAT3& position3
) const noexcept {
	DirectX::XMVECTOR vertex1 = DirectX::XMLoadFloat3(&position1);
	DirectX::XMVECTOR vertex2 = DirectX::XMLoadFloat3(&position2);
	DirectX::XMVECTOR vertex3 = DirectX::XMLoadFloat3(&position3);

	// Edges should be calculated in this subtraction order for ClockWise triangle vertices
	// drawing order
	DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(vertex2, vertex1);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(vertex3, vertex1);

	DirectX::XMVECTOR faceNormalV = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(edge1, edge2)
	);
	DirectX::XMFLOAT3 faceNormal{};
	DirectX::XMStoreFloat3(&faceNormal, faceNormalV);

	return faceNormal;
}

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

// ModelBase
ModelBase::ModelBase() noexcept
	: m_diffuseTexIndex{ 0u }, m_specularTexIndex{ 0u },
	m_diffuseTexUVInfo{ 0.f, 0.f, 1.f, 1.f }, m_specularTexUVInfo{ 0.f, 0.f, 1.f, 1.f },
	m_indexCount{ 0u }, m_indexOffset{ 0u }, m_material{
		.ambient = { 1.f, 1.f, 1.f, 1.f },
		.diffuse = { 1.f, 1.f, 1.f, 1.f },
		.specular = { 1.f, 1.f, 1.f, 1.f }
	}, m_lightSource{ false } {}

void ModelBase::SetDiffuseTexIndex(size_t index) noexcept {
	m_diffuseTexIndex = static_cast<std::uint32_t>(index);
}

void ModelBase::SetSpecularTexIndex(size_t index) noexcept {
	m_specularTexIndex = static_cast<std::uint32_t>(index);
}

void ModelBase::SetBoundingBox(const ModelBoundingBox& boundingBox) noexcept {
	m_boundingBox = boundingBox;
}

void ModelBase::SetDiffuseTexUVInfo(
	float uOffset, float vOffset, float uRatio, float vRatio
) noexcept {
	m_diffuseTexUVInfo.uOffset = uOffset;
	m_diffuseTexUVInfo.vOffset = vOffset;
	m_diffuseTexUVInfo.uRatio = uRatio;
	m_diffuseTexUVInfo.vRatio = vRatio;
}

void ModelBase::SetDiffuseTexUVInfo(const UVInfo& uvInfo) noexcept {
	m_diffuseTexUVInfo = uvInfo;
}

void ModelBase::SetSpecularTexUVInfo(
	float uOffset, float vOffset, float uRatio, float vRatio
) noexcept {
	m_specularTexUVInfo.uOffset = uOffset;
	m_specularTexUVInfo.vOffset = vOffset;
	m_specularTexUVInfo.uRatio = uRatio;
	m_specularTexUVInfo.vRatio = vRatio;
}

void ModelBase::SetSpecularTexUVInfo(const UVInfo& uvInfo) noexcept {
	m_specularTexUVInfo = uvInfo;
}

void ModelBase::SetIndexOffset(std::uint32_t indexOffset) noexcept {
	m_indexOffset = indexOffset;
}

void ModelBase::SetIndexCount(std::uint32_t indexCount) noexcept {
	m_indexCount = indexCount;
}

void ModelBase::SetAsLightSource() noexcept {
	m_lightSource = true;
}

void ModelBase::SetMaterial(const Material& material) noexcept {
	m_material = material;
}

void ModelBase::SetResources() {}

void ModelBase::PhysicsUpdate() noexcept {}

std::uint32_t ModelBase::GetIndexCount() const noexcept {
	return m_indexCount;
}

DirectX::XMMATRIX ModelBase::GetModelMatrix() const noexcept {
	return m_transform.GetModelMatrix();
}

UVInfo ModelBase::GetDiffuseTexUVInfo() const noexcept {
	return m_diffuseTexUVInfo;
}

std::uint32_t ModelBase::GetDiffuseTexIndex() const noexcept {
	return m_diffuseTexIndex;
}

UVInfo ModelBase::GetSpecularTexUVInfo() const noexcept {
	return m_specularTexUVInfo;
}

std::uint32_t ModelBase::GetSpecularTexIndex() const noexcept {
	return m_specularTexIndex;
}

std::uint32_t ModelBase::GetIndexOffset() const noexcept {
	return m_indexOffset;
}

ModelBounds ModelBase::GetBoundingBox() const noexcept {
	return m_boundingBox.GetBoundingCube();
}

DirectX::XMFLOAT3 ModelBase::GetModelOffset() const noexcept {
	return m_transform.GetModelOffset();
}

ModelTransform& ModelBase::GetTransform() noexcept {
	return m_transform;
}

ModelBoundingBox& ModelBase::GetBoundingBox() noexcept {
	return m_boundingBox;
}

Material ModelBase::GetMaterial() const noexcept {
	return m_material;
}

bool ModelBase::IsLightSource() const noexcept {
	return m_lightSource;
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
