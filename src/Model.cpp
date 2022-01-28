#include <cstring>
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

Ceres::Float32_4 Model::GetSolidColor() const noexcept {
	return m_solidColor;
}

void Model::SetSolidColor(const Ceres::VectorF32& colorVector) noexcept {
	m_solidColor = colorVector;
}

const std::vector<VertexElementType>& Model::GetVertexLayout() const noexcept {
	return m_vertexLayout;
}

void Model::ResetVerticesAndIndices() noexcept {
	m_vertices = std::vector<Vertex>();
	m_indices = std::vector<std::uint16_t>();
}
