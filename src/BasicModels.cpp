#include <BasicModels.hpp>
#include <CRSColor.hpp>

Triangle::Triangle() noexcept {
	Init(Ceres::Color::Fuchsia);
}

Triangle::Triangle(const Ceres::VectorF32& solidColor) noexcept {
	Init(solidColor);
}

void Triangle::Init(const Ceres::VectorF32& solidColor) noexcept {
	m_solidColor = solidColor;

	m_vertices.emplace_back(0.25f, 0.25f * (16.0f/9.0f), 0.0f);
	m_vertices.emplace_back(0.50f, -0.25f * (16.0f/9.0f), 0.0f);
	m_vertices.emplace_back(0.0f, -0.25f * (16.0f/9.0f), 0.0f);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	m_vertexLayout.emplace_back(VertexElementType::Color);
}

Triangle1::Triangle1() noexcept {
	Init(Ceres::Color::Fuchsia);
}

Triangle1::Triangle1(const Ceres::VectorF32& solidColor) noexcept {
	Init(solidColor);
}

void Triangle1::Init(const Ceres::VectorF32& solidColor) noexcept {
	m_solidColor = solidColor;

	m_vertices.emplace_back(-0.25f, 0.25f * (16.0f/9.0f), 0.0f);
	m_vertices.emplace_back(0.0f, -0.25f * (16.0f/9.0f), 0.0f);
	m_vertices.emplace_back(-0.50f, -0.25f * (16.0f/9.0f), 0.0f);

	m_indices = { 0u, 1u, 2u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	m_vertexLayout.emplace_back(VertexElementType::Color);
}

