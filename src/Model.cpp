#include <cstring>
#include <Model.hpp>

void Model::GetVertices(
	float* vertices, std::uint32_t& vertexCount
) const noexcept {
	if (vertices)
		std::memcpy(vertices, m_vertices.data(), vertexCount);
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

void Model::GetTransform(float* matrix) const noexcept {

}

void Model::GetSolidColor(float* colorVector) const noexcept {
	std::memcpy(colorVector, &m_solidColor.F32.x, 16u);
}

void Model::SetSolidColor(const float* const colorVector) noexcept {
	m_solidColor.F32.x = colorVector[0];
	m_solidColor.F32.y = colorVector[1];
	m_solidColor.F32.z = colorVector[2];
	m_solidColor.F32.w = colorVector[3];
}
