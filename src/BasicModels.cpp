#include <BasicModels.hpp>
#include <CRSColour.hpp>

Triangle::Triangle() noexcept {
	Init(Ceres::Colour::Fuchsia);
}

Triangle::Triangle(const Ceres::Float32_4 &solidColour) noexcept {
	Init(solidColour);
}

void Triangle::Init(const Ceres::Float32_4& solidColour) noexcept {
	Vertex temp = {};
	temp.position = { 0.25f, 0.25f * (16.0f / 9.0f), 0.0f };
	temp.colour = solidColour;

	m_vertices.emplace_back(temp);
	temp.position = { 0.50f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { 0.0f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	m_vertexLayout.emplace_back(VertexElementType::Colour);
}

Triangle1::Triangle1() noexcept {
	Init(Ceres::Colour::Fuchsia);
}

Triangle1::Triangle1(const Ceres::Float32_4& solidColour) noexcept {
	Init(solidColour);
}

void Triangle1::Init(const Ceres::Float32_4& solidColour) noexcept {
	Vertex temp = {};
	temp.position = { -0.25f, 0.25f * (16.0f/9.0f), 0.0f };
	temp.colour = solidColour;

	m_vertices.emplace_back(temp);
	temp.position = { 0.0f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);
	temp.position = { -0.50f, -0.25f * (16.0f/9.0f), 0.0f };

	m_vertices.emplace_back(temp);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	m_vertexLayout.emplace_back(VertexElementType::Colour);
}

