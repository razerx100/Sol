#include <BasicModels.hpp>

Triangle::Triangle() noexcept {
	Init();
}

void Triangle::Init() noexcept {
	Vertex temp = {};

	temp.position = { 0.25f, 0.25f, 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.50f, -0.25f, 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.0f, -0.25f, 0.0f };

	m_vertices.emplace_back(temp);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	//m_vertexLayout.emplace_back(VertexElementType::UV);
}
