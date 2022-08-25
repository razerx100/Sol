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
	: m_textureIndex(0u), m_modelMatrix(DirectX::XMMatrixIdentity()),
	m_uvInfo{}, m_indexCount{ indexCount }, m_indexOffset{ 0u } {}

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

void Model::SetModelMatrix(const DirectX::XMMATRIX& modelMatrix) noexcept {
	m_modelMatrix = modelMatrix;
}

void Model::AddTransformation(const DirectX::XMMATRIX& transform) noexcept {
	m_modelMatrix *= transform;
}

void Model::SetIndexOffset(std::uint32_t indexOffset) noexcept {
	m_indexOffset = indexOffset;
}

std::uint32_t Model::GetIndexOffset() const noexcept {
	return m_indexOffset;
}
