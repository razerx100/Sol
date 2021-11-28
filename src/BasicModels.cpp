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

	m_vertices.emplace_back(0.0f, -0.5f, 0.0f);
	m_vertices.emplace_back(0.5f, 0.5f, 0.0f);
	m_vertices.emplace_back(-0.5f, 0.5f, 0.0f);

	m_indices = { 0u, 1u, 2u };
}
