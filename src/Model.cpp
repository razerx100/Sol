#include <Model.hpp>

const void* Model::GetVertexData() const noexcept {
	return m_vertices.data();
}

size_t Model::GetVertexStrideSize() const noexcept {
	return sizeof(Vertex);
}

size_t Model::GetVertexBufferSize() const noexcept {
	return GetVertexStrideSize() * m_vertices.size();
}

const void* Model::GetIndexData() const noexcept {
	return m_indices.data();
}

size_t Model::GetIndexBufferSize() const noexcept {
	return sizeof(std::uint16_t) * m_indices.size();
}

size_t Model::GetIndexCount() const noexcept {
	return m_indices.size();
}

Ceres::Matrix Model::GetTransform() const noexcept {
	return m_transform;
}

const std::vector<VertexElementType>& Model::GetVertexLayout() const noexcept {
	return m_vertexLayout;
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

void Model::SetTextureInfo(
	const TextureData& textureInfo
) noexcept {
	m_textureData = textureInfo;
}

const TextureData& Model::GetTextureInfo() const noexcept {
	return m_textureData;
}
