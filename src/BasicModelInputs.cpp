#include <vector>

#include <BasicModelInputs.hpp>

void SetUVToVertices(
	std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
) noexcept {
	for (size_t index = 0u; index < std::size(uvs); ++index)
		vertices[index].uv = uvs[index];
}

// Triangle
TriangleInputs::TriangleInputs() noexcept : ModelInputs{ "Triangle" } {}

void TriangleInputs::InitData() noexcept {
	m_vertices.resize(6);
	m_vertices[0].position = { 0.f, 1.f, 0.f };
	m_vertices[1].position = { -1.f, -1.f, 0.f };
	m_vertices[2].position = { 1.f, -1.f, 0.f };
	m_vertices[3].position = { 0.f, 1.f, 0.f };
	m_vertices[4].position = { -1.f, -1.f, 0.f };
	m_vertices[5].position = { 1.f, -1.f, 0.f };

	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	std::vector<DirectX::XMFLOAT2> uvs { 3u, singleColourUV };

	SetUVToVertices(m_vertices, uvs);

	m_indices = { 0u, 1u, 2u, 5u, 4u, 3u };

	CalculateNormalsIndependentFaces();
}

// Cube
CubeInputs::CubeInputs(const Args& args) noexcept : m_uvMode{ args.uvMode.value() } {
	std::string name = "Cube";
	switch (args.uvMode.value()) {
	case CubeUVMode::SingleColour: {
		name.append("-SingleColour");
		break;
	}
	case CubeUVMode::IndependentFaceTexture: {
		name.append("-IndependentFaceTexture");
		break;
	}
	}

	SetInputName(name);
}

void CubeInputs::SetSingleColourUV() noexcept {
	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	std::vector<DirectX::XMFLOAT2> uvs{ 24u, singleColourUV };

	SetUVToVertices(m_vertices, uvs);
}

void CubeInputs::SetIndependentFaceTexUV() noexcept {
	std::vector<DirectX::XMFLOAT2> uvs{};

	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 1.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 0.f, 0.f });
	uvs.emplace_back(DirectX::XMFLOAT2{ 1.f, 0.f });

	SetUVToVertices(m_vertices, uvs);
}

void CubeInputs::InitData() noexcept {
	constexpr float side = 1.0f / 2.0f;

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

	switch (m_uvMode) {
	case CubeUVMode::SingleColour: {
		SetSingleColourUV();
		break;
	}
	case CubeUVMode::IndependentFaceTexture: {
		SetIndependentFaceTexUV();
		break;
	}
	}

	m_indices = {
			0u, 2u, 1u,			2u, 3u, 1u,
			4u, 5u, 7u,			4u, 7u, 6u,
			8u, 10u, 9u,		10u, 11u, 9u,
			12u, 13u, 15u,		12u, 15u, 14u,
			16u, 17u, 18u,		18u, 17u, 19u,
			20u, 23u, 21u,		20u, 22u, 23u
	};

	CalculateNormalsIndependentFaces();
}

// Quad
QuadInputs::QuadInputs() noexcept : ModelInputs{ "Quad" } {}

void QuadInputs::InitData() noexcept {
	constexpr float side = 1.0f / 2.0f;

	m_vertices.resize(8u);
	m_vertices[0].position = { -side, side, 0.f };
	m_vertices[1].position = { side, side, 0.f };
	m_vertices[2].position = { -side, -side, 0.f };
	m_vertices[3].position = { side, -side, 0.f };
	m_vertices[4].position = { -side, side, 0.f };
	m_vertices[5].position = { side, side, 0.f };
	m_vertices[6].position = { -side, -side, 0.f };
	m_vertices[7].position = { side, -side, 0.f };

	std::vector<DirectX::XMFLOAT2> uvs {
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

	CalculateNormalsIndependentFaces();
}

// Sphere
SphereInputs::SphereInputs(const Args& arguments) noexcept
	: ModelInputs{
		"Sphere" + std::to_string(arguments.latDiv.value()) + "x" +
		std::to_string(arguments.longDiv.value())
	},
	m_latitudeDivision{ arguments.latDiv.value() },
	m_longitudeDivision{ arguments.longDiv.value() } {}

void SphereInputs::InitData() noexcept {
	assert(m_latitudeDivision > 2);
	assert(m_longitudeDivision > 2);

	constexpr float radius = 1.f;
	const DirectX::XMVECTOR base = DirectX::XMVectorSet(0.f, 0.f, radius, 0.f);
	const float lattitudeAngle = DirectX::XM_PI / m_latitudeDivision;
	const float longitudeAngle = DirectX::XM_2PI / m_longitudeDivision;

	// Vertices
	for (std::uint32_t iLat = 1u; iLat < m_latitudeDivision; iLat++) {
		DirectX::XMVECTOR latBase = DirectX::XMVector3Transform(
			base, DirectX::XMMatrixRotationX(lattitudeAngle * iLat)
		);

		for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision; iLong++) {
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(
				latBase, DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);

			Vertex vertex{};
			DirectX::XMStoreFloat3(&vertex.position, v);
			m_vertices.emplace_back(vertex);
		}
	}

	// Cap vertices
	const auto iNorthPole = static_cast<std::uint32_t>(std::size(m_vertices));
	Vertex vertex{};
	DirectX::XMStoreFloat3(&vertex.position, base);
	m_vertices.emplace_back(vertex);

	const auto iSouthPole = static_cast<std::uint32_t>(std::size(m_vertices));
	DirectX::XMStoreFloat3(&vertex.position, DirectX::XMVectorNegate(base));
	m_vertices.emplace_back(vertex);

	// UV
	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	std::vector<DirectX::XMFLOAT2> uvs { std::size(m_vertices), singleColourUV};

	SetUVToVertices(m_vertices, uvs);

	// Normals
	CalculateNormals();

	// Indices
	const auto calcIndex = [longDiv = m_longitudeDivision]
	(std::uint32_t iLat, std::uint32_t iLong)->std::uint32_t {
		return iLat * longDiv + iLong;
	};

	for (std::uint32_t iLat = 0u; iLat < m_latitudeDivision - 2u; iLat++) {
		for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision - 1u; iLong++) {
			m_indices.emplace_back(calcIndex(iLat, iLong));
			m_indices.emplace_back(calcIndex(iLat + 1u, iLong));
			m_indices.emplace_back(calcIndex(iLat, iLong + 1u));
			m_indices.emplace_back(calcIndex(iLat, iLong + 1u));
			m_indices.emplace_back(calcIndex(iLat + 1u, iLong));
			m_indices.emplace_back(calcIndex(iLat + 1u, iLong + 1u));
		}
		// Wrap band
		m_indices.emplace_back(calcIndex(iLat, m_longitudeDivision - 1u));
		m_indices.emplace_back(calcIndex(iLat + 1u, m_longitudeDivision - 1u));
		m_indices.emplace_back(calcIndex(iLat, 0u));
		m_indices.emplace_back(calcIndex(iLat, 0u));
		m_indices.emplace_back(calcIndex(iLat + 1u, m_longitudeDivision - 1u));
		m_indices.emplace_back(calcIndex(iLat + 1u, 0u));
	}

	// Cap fans
	for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision - 1u; iLong++) {
		// North
		m_indices.emplace_back(iNorthPole);
		m_indices.emplace_back(calcIndex(0u, iLong));
		m_indices.emplace_back(calcIndex(0u, iLong + 1u));
		// South
		m_indices.emplace_back(calcIndex(m_latitudeDivision - 2u, iLong + 1u));
		m_indices.emplace_back(calcIndex(m_latitudeDivision - 2u, iLong));
		m_indices.emplace_back(iSouthPole);
	}
	// Wrap Triangles
	// North
	m_indices.emplace_back(iNorthPole);
	m_indices.emplace_back(calcIndex(0u, m_longitudeDivision - 1u));
	m_indices.emplace_back(calcIndex(0u, 0u));
	// South
	m_indices.emplace_back(calcIndex(m_latitudeDivision - 2u, 0u));
	m_indices.emplace_back(calcIndex(m_latitudeDivision - 2u, m_longitudeDivision - 1u));
	m_indices.emplace_back(iSouthPole);
}

void SphereInputs::CalculateNormals() noexcept {
	// A sphere's vertices are facing towards the outside from the centre. So, the normal
	// of a vertex would be vertex's position vector - the position vector of the centre.
	// Since the centre is at (0, 0, 0), the normal of a vertex would be its unit vector
	for (Vertex& vertex : m_vertices) {
		DirectX::XMVECTOR unitVector = DirectX::XMVector3Normalize(
			DirectX::XMLoadFloat3(&vertex.position)
		);

		DirectX::XMStoreFloat3(&vertex.normal, unitVector);
	}
}
