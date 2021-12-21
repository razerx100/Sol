#include <cstring>
#include <Model.hpp>

const std::vector<Ceres::Float32_3>& Model::GetVertices() const noexcept {
	return m_vertices;
}

const std::vector<std::uint16_t>& Model::GetIndices() const noexcept {
	return m_indices;
}

Ceres::Matrix Model::GetTransform() const noexcept {
	return m_transform;
}

Ceres::VectorF32 Model::GetSolidColor() const noexcept {
	return m_solidColor;
}

void Model::SetSolidColor(const Ceres::VectorF32& colorVector) noexcept {
	m_solidColor = colorVector;
}

const std::vector<VertexElementType>& Model::GetVertexLayout() const noexcept {
	return m_vertexLayout;
}

void Model::ResetVerticesAndIndices() noexcept {
	m_vertices = std::vector<Ceres::Float32_3>();
	m_indices = std::vector<std::uint16_t>();
}
