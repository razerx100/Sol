#include <Model.hpp>

Model::Model() noexcept
	: m_textureIndex(0u), m_modelMatrix(DirectX::XMMatrixIdentity()), m_uvInfo{} {}

std::unique_ptr<std::uint8_t> Model::GetVertexData() const noexcept {
	return GetDataFromVector(m_vertices);
}

size_t Model::GetVertexStrideSize() const noexcept {
	return sizeof(Vertex);
}

size_t Model::GetVertexBufferSize() const noexcept {
	return GetVertexStrideSize() * std::size(m_vertices);
}

std::unique_ptr<std::uint8_t> Model::GetIndexData() const noexcept {
	return GetDataFromVector(m_indices);
}

size_t Model::GetIndexBufferSize() const noexcept {
	return sizeof(std::uint16_t) * std::size(m_indices);
}

size_t Model::GetIndexCount() const noexcept {
	return std::size(m_indices);
}

DirectX::XMMATRIX Model::GetModelMatrix() const noexcept {
	return m_modelMatrix;
}

void Model::ResetVerticesAndIndices() noexcept {
	m_vertices = std::vector<Vertex>();
	m_indices = std::vector<std::uint16_t>();
}

void Model::SetTextureIndex(size_t index) noexcept {
	m_textureIndex = static_cast<std::uint32_t>(index);
}

std::uint32_t Model::GetTextureIndex() const noexcept {
	return m_textureIndex;
}

void Model::SetUVInfo(float uOffset, float vOffset, float uRatio, float vRatio) noexcept {
	m_uvInfo.u = uOffset;
	m_uvInfo.v = vOffset;
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
