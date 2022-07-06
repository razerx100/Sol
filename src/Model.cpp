#include <Model.hpp>

std::unique_ptr<std::uint8_t> ModelInputs::GetVertexData() const noexcept {
	return GetDataFromVector(m_vertices);
}

size_t ModelInputs::GetVertexStrideSize() const noexcept {
	return sizeof(Vertex);
}

size_t ModelInputs::GetVertexBufferSize() const noexcept {
	return GetVertexStrideSize() * std::size(m_vertices);
}

std::unique_ptr<std::uint8_t> ModelInputs::GetIndexData() const noexcept {
	return GetDataFromVector(m_indices);
}

size_t ModelInputs::GetIndexBufferSize() const noexcept {
	return sizeof(std::uint16_t) * std::size(m_indices);
}

std::uint32_t ModelInputs::GetIndexCount() const noexcept {
	return static_cast<std::uint32_t>(std::size(m_indices));
}

// Model
Model::Model() noexcept
	: m_textureIndex(0u), m_modelMatrix(DirectX::XMMatrixIdentity()),
	m_uvInfo{}, m_indexCount(0u) {}

void Model::SetIndexCount(std::uint32_t indexCount) noexcept {
	m_indexCount = indexCount;
}

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
