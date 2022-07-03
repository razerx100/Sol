#include <BasicModels.hpp>

#include <DirectXMath.h>

void SetUVToVertices(
	std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
) noexcept {
	for (size_t index = 0u; index < std::size(uvs); ++index)
		vertices[index].uv = uvs[index];
}

Triangle::Triangle() noexcept {
	Init();
}

void Triangle::Init() noexcept {
	m_vertices.resize(3);
	m_vertices[0].position = { 0.f, 1.f, 0.f };
	m_vertices[1].position = { -1.f, -1.f, 0.f };
	m_vertices[2].position = { 1.f, -1.f, 0.f };

	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	static std::vector<DirectX::XMFLOAT2> uvs { 3u, singleColourUV };

	SetUVToVertices(m_vertices, uvs);

	m_modelMatrix *= DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_indices = { 0u, 1u, 2u, 2u, 1u, 0u };
}

Cube::Cube() noexcept {
	Init();
}

void Cube::Init() noexcept {

	constexpr float side = 1.0f / 2.0f;

	m_modelMatrix *= DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_vertices.resize(24);
	m_vertices[0].position = { -side,-side,-side };// 0 near side
	m_vertices[1].position = { side,-side,-side };// 1
	m_vertices[2].position = { -side,side,-side };// 2
	m_vertices[3].position = { side,side,-side };// 3
	m_vertices[4].position = { -side,-side,side };// 4 far side
	m_vertices[5].position = { side,-side,side };// 5
	m_vertices[6].position = { -side,side,side };// 6
	m_vertices[7].position = { side,side,side };// 7
	m_vertices[8].position = { -side,-side,-side };// 8 left side
	m_vertices[9].position = { -side,side,-side };// 9
	m_vertices[10].position = { -side,-side,side };// 10
	m_vertices[11].position = { -side,side,side };// 11
	m_vertices[12].position = { side,-side,-side };// 12 right side
	m_vertices[13].position = { side,side,-side };// 13
	m_vertices[14].position = { side,-side,side };// 14
	m_vertices[15].position = { side,side,side };// 15
	m_vertices[16].position = { -side,-side,-side };// 16 bottom side
	m_vertices[17].position = { side,-side,-side };// 17
	m_vertices[18].position = { -side,-side,side };// 18
	m_vertices[19].position = { side,-side,side };// 19
	m_vertices[20].position = { -side,side,-side };// 20 top side
	m_vertices[21].position = { side,side,-side };// 21
	m_vertices[22].position = { -side,side,side };// 22
	m_vertices[23].position = { side,side,side };// 23

	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	static std::vector<DirectX::XMFLOAT2> uvs { 24u, singleColourUV };

	SetUVToVertices(m_vertices, uvs);

	m_indices = {
			0u, 2u, 1u,			2u, 3u, 1u,
			4u, 5u, 7u,			4u, 7u, 6u,
			8u, 10u, 9u,		10u, 11u, 9u,
			12u, 13u, 15u,		12u, 15u, 14u,
			16u, 17u, 18u,		18u, 17u, 19u,
			20u, 23u, 21u,		20u, 22u, 23u
	};
}

Quad::Quad() noexcept {
	Init();
}

void Quad::Init() noexcept {
	constexpr float side = 1.0f / 2.0f;

	m_modelMatrix *= DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_vertices.resize(8u);
	m_vertices[0].position = { -side, side, 0.f };
	m_vertices[1].position = { side, side, 0.f };
	m_vertices[2].position = { -side, -side, 0.f };
	m_vertices[3].position = { side, -side, 0.f };
	m_vertices[4].position = { -side, side, 0.f };
	m_vertices[5].position = { side, side, 0.f };
	m_vertices[6].position = { -side, -side, 0.f };
	m_vertices[7].position = { side, -side, 0.f };

	static std::vector<DirectX::XMFLOAT2> uvs {
		{0.f, 0.f}, {1.f, 0.f},
		{0.f, 1.f}, {1.f, 1.f},
		{1.f, 0.f}, {0.f, 0.f},
		{1.f, 1.f}, {0.f, 1.f}
	};
	SetUVToVertices(m_vertices, uvs);

	m_indices = {
		0u, 1u, 2u, 2u, 1u, 3u,
		6u, 7u, 5u, 5u, 4u, 6u
	};
}
