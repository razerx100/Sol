#include <BasicModels.hpp>
#include <DirectXColors.h>

Triangle::Triangle() noexcept {
	Init(DirectX::Colors::Aqua);
}

Triangle::Triangle(const DirectX::XMVECTORF32& solidColor) noexcept {
	Init(solidColor);
}

void Triangle::Init(const DirectX::XMVECTORF32& solidColor) noexcept {
	m_solidColor = solidColor;

	m_vertices.emplace_back(0.0f, -0.5f, 0.0f);
	m_vertices.emplace_back(0.5f, 0.5f, 0.0f);
	m_vertices.emplace_back(-0.5f, 0.5f, 0.0f);

	m_indices = { 0u, 1u, 2u };
}
