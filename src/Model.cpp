#include <cstring>
#include <Model.hpp>

void Model::GetVertices(
	Ceres::Float32_3* vertices, std::uint32_t& vertexCount
) const noexcept {
	if (vertices)
		std::memcpy(vertices, m_vertices.data(), vertexCount * sizeof(Ceres::Float32_3));
	else
		vertexCount = static_cast<std::uint32_t>(m_vertices.size());
}

void Model::GetIndices(
	std::uint16_t* indices, std::uint32_t& indexCount
) const noexcept {
	if (indices)
		std::memcpy(indices, m_indices.data(), indexCount);
	else
		indexCount = static_cast<std::uint32_t>(m_indices.size());
}

void Model::GetTransform(Ceres::Matrix& transform) const noexcept {
	transform = m_transform;
}

void Model::GetSolidColor(Ceres::VectorF32& colorVector) const noexcept {
	colorVector = m_solidColor;
}

void Model::SetSolidColor(const Ceres::VectorF32& colorVector) noexcept {
	m_solidColor = colorVector;
}
