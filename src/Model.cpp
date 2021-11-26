#include <cstring>
#include <Model.hpp>

void Model::GetVertices(
	DirectX::XMFLOAT3* vertices, std::uint32_t& vertexCount
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

DirectX::XMMATRIX Model::GetTransform() const noexcept {
	return m_transform;
}

DirectX::XMVECTORF32 Model::GetSolidColor() const noexcept {
	return m_solidColor;
}

void Model::SetSolidColor(const DirectX::XMVECTORF32& color) noexcept {
	m_solidColor = color;
}
