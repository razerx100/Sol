#include <BasicModels.hpp>

Triangle::Triangle() noexcept {
	Init();
}

void Triangle::Init() noexcept {
	Vertex temp = {};

	temp.position = { 0.25f, 0.25f * (16.0f / 9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.50f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.0f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	//m_vertexLayout.emplace_back(VertexElementType::UV);
}

Triangle1::Triangle1() noexcept {
	Init();
}

void Triangle1::Init() noexcept {
	Vertex temp = {};

	temp.position = { -0.25f, 0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.0f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { -0.50f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	//m_vertexLayout.emplace_back(VertexElementType::UV);
}

