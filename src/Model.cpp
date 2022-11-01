#include <Model.hpp>

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
	: m_textureIndex{ 0u }, m_modelMatrix{ DirectX::XMMatrixIdentity() },
	m_uvInfo{}, m_indexCount{ indexCount }, m_indexOffset{ 0u },
	m_modelOffset{ 0.f, 0.f, 0.f } {}

std::uint32_t Model::GetIndexCount() const noexcept {
	return m_indexCount;
}

DirectX::XMMATRIX Model::GetModelMatrix() const noexcept {
	return m_modelMatrix;
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

void Model::Rotate(const DirectX::XMVECTOR& rotationAxis, float angleRadian) noexcept {
	m_modelMatrix *= DirectX::XMMatrixRotationAxis(rotationAxis, angleRadian);
}

DirectX::XMFLOAT3 Model::GetModelOffset() const noexcept {
	return m_modelOffset;
}

Model& Model::RotateXDegree(float angle) noexcept {
	Rotate({ 1.f, 0.f, 0.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

Model& Model::RotateYDegree(float angle) noexcept {
	Rotate({ 0.f, 1.f, 0.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

Model& Model::RotateZDegree(float angle) noexcept {
	Rotate({ 0.f, 0.f, 1.f, 0.f }, DirectX::XMConvertToRadians(angle));

	return *this;
}

Model& Model::RotateXRadian(float angle) noexcept {
	Rotate({ 1.f, 0.f, 0.f, 0.f }, angle);

	return *this;
}

Model& Model::RotateYRadian(float angle) noexcept {
	Rotate({ 0.f, 1.f, 0.f, 0.f }, angle);

	return *this;
}

Model& Model::RotateZRadian(float angle) noexcept {
	Rotate({ 0.f, 0.f, 1.f, 0.f }, angle);

	return *this;
}

Model& Model::MoveTowardsX(float delta) noexcept {
	m_modelOffset.x += delta;

	return *this;
}

Model& Model::MoveTowardsY(float delta) noexcept {
	m_modelOffset.y += delta;

	return *this;
}

Model& Model::MoveTowardsZ(float delta) noexcept {
	m_modelOffset.z += delta;

	return *this;
}
