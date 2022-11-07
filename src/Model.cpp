#include <Model.hpp>
#include <ranges>
#include <algorithm>

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
Model::Model(std::uint32_t indexCount) noexcept
	: m_textureIndex{ 0u }, m_uvInfo{}, m_indexCount{ indexCount }, m_indexOffset{ 0u } {}

std::uint32_t Model::GetIndexCount() const noexcept {
	return m_indexCount;
}

DirectX::XMMATRIX Model::GetModelMatrix() const noexcept {
	return m_transform.GetModelMatrix();
}

void Model::SetTextureIndex(size_t index) noexcept {
	m_textureIndex = static_cast<std::uint32_t>(index);
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

std::uint32_t Model::GetIndexOffset() const noexcept {
	return m_indexOffset;
}

DirectX::XMMATRIX& Model::GetModelMatrixRef() noexcept {
	return m_transform.GetModelMatrixRef();
}

std::array<DirectX::XMFLOAT3, 8u> Model::GetBoundingBox() const noexcept {
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
	DirectX::XMFLOAT3 maxPositiveAxes{};
	DirectX::XMFLOAT3 maxNegativeAxes{};

	for (const auto& vertex : vertices) {
		maxPositiveAxes.x = std::max(vertex.position.x, maxPositiveAxes.x);
		maxPositiveAxes.y = std::max(vertex.position.y, maxPositiveAxes.y);
		maxPositiveAxes.z = std::max(vertex.position.z, maxPositiveAxes.z);

		maxNegativeAxes.x = std::min(vertex.position.x, maxNegativeAxes.x);
		maxNegativeAxes.y = std::min(vertex.position.y, maxNegativeAxes.y);
		maxNegativeAxes.z = std::min(vertex.position.z, maxNegativeAxes.z);
	}

	// -x, +y, -z -> +x, +y, -z
	// -x, -y, -z -> +x, -y, -z
	// -x, +y, +z -> +x, +y, +z
	// -x, -y, +z -> +x, -y, +z
	m_boundingCube[0] = { maxNegativeAxes.x, maxPositiveAxes.y, maxNegativeAxes.z };
	m_boundingCube[1] = { maxPositiveAxes.x, maxPositiveAxes.y, maxNegativeAxes.z };
	m_boundingCube[2] = { maxPositiveAxes.x, maxNegativeAxes.y, maxNegativeAxes.z };
	m_boundingCube[3] = { maxNegativeAxes.x, maxNegativeAxes.y, maxNegativeAxes.z };
	m_boundingCube[4] = { maxNegativeAxes.x, maxPositiveAxes.y, maxPositiveAxes.z };
	m_boundingCube[5] = { maxPositiveAxes.x, maxPositiveAxes.y, maxPositiveAxes.z };
	m_boundingCube[6] = { maxPositiveAxes.x, maxNegativeAxes.y, maxPositiveAxes.z };
	m_boundingCube[7] = { maxNegativeAxes.x, maxNegativeAxes.y, maxPositiveAxes.z };
}

void ModelBoundingBox::SetBoundingCube(const CubeType& cube) noexcept{
	m_boundingCube = cube;
}

const ModelBoundingBox::CubeType& ModelBoundingBox::GetBoundingCube() const noexcept {
	return m_boundingCube;
}

// Model Transform
ModelTransform::ModelTransform() noexcept
	: m_modelMatrix{ DirectX::XMMatrixIdentity() }, m_modelOffset{ 0.f, 0.f, 0.f } {}

DirectX::XMMATRIX& ModelTransform::GetModelMatrixRef() noexcept {
	return m_modelMatrix;
}

DirectX::XMFLOAT3& ModelTransform::GetModelOffsetRef() noexcept {
	return m_modelOffset;
}

DirectX::XMMATRIX ModelTransform::GetModelMatrix() const noexcept {
	return m_modelMatrix;
}

DirectX::XMFLOAT3 ModelTransform::GetModelOffset() const noexcept {
	return m_modelOffset;
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
