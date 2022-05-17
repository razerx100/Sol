#include <BasicModels.hpp>

#include <DirectXMath.h>

Triangle::Triangle() noexcept {
	Init();
}

void Triangle::Init() noexcept {
	m_vertices = {
		{{ 0.f, 1.f, 0.f }},
		{{ -1.f, -1.f, 0.f }},
		{{ 1.f, -1.f, 0.f }}
	};

	m_modelMatrix *= DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_indices = { 0u, 1u, 2u, 2u, 1u, 0u };

	m_vertexLayout.emplace_back(VertexElementType::Position);
	//m_vertexLayout.emplace_back(VertexElementType::UV);
}

Cube::Cube() noexcept {
	Init();
}

void Cube::Init() noexcept {

	constexpr float side = 1.0f / 2.0f;

	m_modelMatrix *= DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_vertices.resize(24);
	m_vertices[0] = { { -side,-side,-side } };// 0 near side
	m_vertices[1] = { { side,-side,-side } };// 1
	m_vertices[2] = { { -side,side,-side } };// 2
	m_vertices[3] = { { side,side,-side } };// 3
	m_vertices[4] = { { -side,-side,side } };// 4 far side
	m_vertices[5] = { { side,-side,side } };// 5
	m_vertices[6] = { { -side,side,side } };// 6
	m_vertices[7] = { { side,side,side } };// 7
	m_vertices[8] = { { -side,-side,-side } };// 8 left side
	m_vertices[9] = { { -side,side,-side } };// 9
	m_vertices[10] = { { -side,-side,side } };// 10
	m_vertices[11] = { { -side,side,side } };// 11
	m_vertices[12] = { { side,-side,-side } };// 12 right side
	m_vertices[13] = { { side,side,-side } };// 13
	m_vertices[14] = { { side,-side,side } };// 14
	m_vertices[15] = { { side,side,side } };// 15
	m_vertices[16] = { { -side,-side,-side } };// 16 bottom side
	m_vertices[17] = { { side,-side,-side } };// 17
	m_vertices[18] = { { -side,-side,side } };// 18
	m_vertices[19] = { { side,-side,side } };// 19
	m_vertices[20] = { { -side,side,-side } };// 20 top side
	m_vertices[21] = { { side,side,-side } };// 21
	m_vertices[22] = { { -side,side,side } };// 22
	m_vertices[23] = { { side,side,side } };// 23

	m_indices = {
			0u, 2u, 1u,			2u, 3u, 1u,
			4u, 5u, 7u,			4u, 7u, 6u,
			8u, 10u, 9u,		10u, 11u, 9u,
			12u, 13u, 15u,		12u, 15u, 14u,
			16u, 17u, 18u,		18u, 17u, 19u,
			20u, 23u, 21u,		20u, 22u, 23u
	};

	m_vertexLayout.emplace_back(VertexElementType::Position);
	//m_vertexLayout.emplace_back(VertexElementType::UV);
}
