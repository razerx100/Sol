#include <BasicMeshBundles.hpp>
#include <format>

// Triangle
void TriangleMesh::SetMesh(Mesh& mesh) noexcept
{
	std::vector<Vertex>& vertices = mesh.vertices;

	vertices.resize(6);
	vertices[0].position = { 0.f, 1.f, 0.f };
	vertices[1].position = { -1.f, -1.f, 0.f };
	vertices[2].position = { 1.f, -1.f, 0.f };
	vertices[3].position = { 0.f, 1.f, 0.f };
	vertices[4].position = { -1.f, -1.f, 0.f };
	vertices[5].position = { 1.f, -1.f, 0.f };

	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	std::vector<DirectX::XMFLOAT2> uvs { 3u, singleColourUV };

	MeshBundleImpl::SetUVToVertices(vertices, uvs);

	std::vector<std::uint32_t>& indices = mesh.indices;

	indices = { 0u, 1u, 2u, 5u, 4u, 3u };

	MeshBundleImpl::CalculateNormalsIndependentFaces(vertices, indices);
}

// Cube
void CubeMesh::SetMesh(Mesh& mesh, CubeUVMode uvMode) noexcept
{
	constexpr float side = 1.0f / 2.0f;

	std::vector<Vertex>& vertices = mesh.vertices;

	vertices.resize(24);
	vertices[0].position  = { -side,-side,-side };// 0 near side
	vertices[1].position  = { side,-side,-side };// 1
	vertices[2].position  = { -side,side,-side };// 2
	vertices[3].position  = { side,side,-side };// 3
	vertices[4].position  = { -side,-side,side };// 4 far side
	vertices[5].position  = { side,-side,side };// 5
	vertices[6].position  = { -side,side,side };// 6
	vertices[7].position  = { side,side,side };// 7
	vertices[8].position  = { -side,-side,-side };// 8 left side
	vertices[9].position  = { -side,side,-side };// 9
	vertices[10].position = { -side,-side,side };// 10
	vertices[11].position = { -side,side,side };// 11
	vertices[12].position = { side,-side,-side };// 12 right side
	vertices[13].position = { side,side,-side };// 13
	vertices[14].position = { side,-side,side };// 14
	vertices[15].position = { side,side,side };// 15
	vertices[16].position = { -side,-side,-side };// 16 bottom side
	vertices[17].position = { side,-side,-side };// 17
	vertices[18].position = { -side,-side,side };// 18
	vertices[19].position = { side,-side,side };// 19
	vertices[20].position = { -side,side,-side };// 20 top side
	vertices[21].position = { side,side,-side };// 21
	vertices[22].position = { -side,side,side };// 22
	vertices[23].position = { side,side,side };// 23

	if (uvMode == CubeUVMode::SingleColour)
		SetSingleColourUV(vertices);
	else if (uvMode == CubeUVMode::IndependentFaceTexture)
		SetIndependentFaceTexUV(vertices);

	std::vector<std::uint32_t>& indices = mesh.indices;

	indices = {
			0u,  2u,  1u,   2u,  3u,  1u,
			4u,  5u,  7u,   4u,  7u,  6u,
			8u,  10u, 9u,   10u, 11u, 9u,
			12u, 13u, 15u,  12u, 15u, 14u,
			16u, 17u, 18u,  18u, 17u, 19u,
			20u, 23u, 21u,  20u, 22u, 23u
	};

	MeshBundleImpl::CalculateNormalsIndependentFaces(vertices, indices);
}

std::string CubeMesh::GetName(CubeUVMode uvMode) noexcept
{
	std::string name = "Cube";

	if (uvMode == CubeUVMode::SingleColour)
		name.append("-SingleColour");
	else if (uvMode == CubeUVMode::IndependentFaceTexture)
		name.append("-IndependentFaceTexture");

	return name;
}

void CubeMesh::SetSingleColourUV(std::vector<Vertex>& vertices) noexcept
{
	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };

	std::vector<DirectX::XMFLOAT2> uvs{ 24u, singleColourUV };

	MeshBundleImpl::SetUVToVertices(vertices, uvs);
}

void CubeMesh::SetIndependentFaceTexUV(std::vector<Vertex>& vertices) noexcept
{
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

	MeshBundleImpl::SetUVToVertices(vertices, uvs);
}

// Quad
void QuadMesh::SetMesh(Mesh& mesh) noexcept
{
	std::vector<Vertex>& vertices = mesh.vertices;

	constexpr float side = 1.0f / 2.0f;

	vertices.resize(8u);
	vertices[0].position = { -side, side, 0.f };
	vertices[1].position = { side, side, 0.f };
	vertices[2].position = { -side, -side, 0.f };
	vertices[3].position = { side, -side, 0.f };
	vertices[4].position = { -side, side, 0.f };
	vertices[5].position = { side, side, 0.f };
	vertices[6].position = { -side, -side, 0.f };
	vertices[7].position = { side, -side, 0.f };

	std::vector<DirectX::XMFLOAT2> uvs
	{
		{ 0.f, 0.f }, { 1.f, 0.f },
		{ 0.f, 1.f }, { 1.f, 1.f },
		{ 1.f, 0.f }, { 0.f, 0.f },
		{ 1.f, 1.f }, { 0.f, 1.f }
	};

	MeshBundleImpl::SetUVToVertices(vertices, uvs);

	std::vector<std::uint32_t>& indices = mesh.indices;

	indices = {
		0u, 1u, 2u, 2u, 1u, 3u,
		6u, 7u, 5u, 5u, 4u, 6u
	};

	MeshBundleImpl::CalculateNormalsIndependentFaces(vertices, indices);
}

// Render Target Quad Mesh
void RenderTargetQuadMesh::SetMesh(Mesh& mesh) noexcept
{
	std::vector<Vertex>& vertices = mesh.vertices;

	constexpr float side = 1.0f;

	vertices.resize(4u);

	vertices[0].position = { -side, side, 0.f };
	vertices[1].position = { side, side, 0.f };
	vertices[2].position = { -side, -side, 0.f };
	vertices[3].position = { side, -side, 0.f };

	std::vector<DirectX::XMFLOAT2> uvs
	{
		{ 0.f, 0.f }, { 1.f, 0.f },
		{ 0.f, 1.f }, { 1.f, 1.f }
	};

	MeshBundleImpl::SetUVToVertices(vertices, uvs);

	std::vector<std::uint32_t>& indices = mesh.indices;

	indices = { 0u, 1u, 2u, 2u, 1u, 3u };

	MeshBundleImpl::CalculateNormalsIndependentFaces(vertices, indices);
}

// Sphere
std::string SphereMesh::GetName(std::uint32_t latDiv, std::uint32_t longDiv) noexcept
{
	return std::format("Sphere{}{}", latDiv, longDiv);
}

void SphereMesh::SetMesh(Mesh& mesh) noexcept
{
	assert(m_latitudeDivision > 2);
	assert(m_longitudeDivision > 2);

	std::vector<Vertex>& vertices = mesh.vertices;

	constexpr float radius       = 1.f;
	const DirectX::XMVECTOR base = DirectX::XMVectorSet(0.f, 0.f, radius, 0.f);
	const float lattitudeAngle   = DirectX::XM_PI / m_latitudeDivision;
	const float longitudeAngle   = DirectX::XM_2PI / m_longitudeDivision;

	// Vertices
	for (std::uint32_t iLat = 1u; iLat < m_latitudeDivision; iLat++)
	{
		DirectX::XMVECTOR latBase = DirectX::XMVector3Transform(
			base, DirectX::XMMatrixRotationX(lattitudeAngle * iLat)
		);

		for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision; iLong++)
		{
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(
				latBase, DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);

			Vertex vertex{};
			DirectX::XMStoreFloat3(&vertex.position, v);
			vertices.emplace_back(vertex);
		}
	}

	// Cap vertices
	const auto iNorthPole = static_cast<std::uint32_t>(std::size(vertices));
	Vertex vertex{};
	DirectX::XMStoreFloat3(&vertex.position, base);
	vertices.emplace_back(vertex);

	const auto iSouthPole = static_cast<std::uint32_t>(std::size(vertices));
	DirectX::XMStoreFloat3(&vertex.position, DirectX::XMVectorNegate(base));
	vertices.emplace_back(vertex);

	// UV
	constexpr DirectX::XMFLOAT2 singleColourUV { 0.f, 0.f };
	std::vector<DirectX::XMFLOAT2> uvs { std::size(vertices), singleColourUV};

	MeshBundleImpl::SetUVToVertices(vertices, uvs);

	// Normals
	CalculateNormals(vertices);

	// Indices
	const auto calcIndex = [longDiv = m_longitudeDivision]
	(std::uint32_t iLat, std::uint32_t iLong)->std::uint32_t {
		return iLat * longDiv + iLong;
	};

	std::vector<std::uint32_t>& indices = mesh.indices;

	for (std::uint32_t iLat = 0u; iLat < m_latitudeDivision - 2u; iLat++)
	{
		for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision - 1u; iLong++)
		{
			indices.emplace_back(calcIndex(iLat, iLong));
			indices.emplace_back(calcIndex(iLat + 1u, iLong));
			indices.emplace_back(calcIndex(iLat, iLong + 1u));
			indices.emplace_back(calcIndex(iLat, iLong + 1u));
			indices.emplace_back(calcIndex(iLat + 1u, iLong));
			indices.emplace_back(calcIndex(iLat + 1u, iLong + 1u));
		}
		// Wrap band
		indices.emplace_back(calcIndex(iLat, m_longitudeDivision - 1u));
		indices.emplace_back(calcIndex(iLat + 1u, m_longitudeDivision - 1u));
		indices.emplace_back(calcIndex(iLat, 0u));
		indices.emplace_back(calcIndex(iLat, 0u));
		indices.emplace_back(calcIndex(iLat + 1u, m_longitudeDivision - 1u));
		indices.emplace_back(calcIndex(iLat + 1u, 0u));
	}

	// Cap fans
	for (std::uint32_t iLong = 0u; iLong < m_longitudeDivision - 1u; iLong++)
	{
		// North
		indices.emplace_back(iNorthPole);
		indices.emplace_back(calcIndex(0u, iLong));
		indices.emplace_back(calcIndex(0u, iLong + 1u));
		// South
		indices.emplace_back(calcIndex(m_latitudeDivision - 2u, iLong + 1u));
		indices.emplace_back(calcIndex(m_latitudeDivision - 2u, iLong));
		indices.emplace_back(iSouthPole);
	}
	// Wrap Triangles
	// North
	indices.emplace_back(iNorthPole);
	indices.emplace_back(calcIndex(0u, m_longitudeDivision - 1u));
	indices.emplace_back(calcIndex(0u, 0u));
	// South
	indices.emplace_back(calcIndex(m_latitudeDivision - 2u, 0u));
	indices.emplace_back(calcIndex(m_latitudeDivision - 2u, m_longitudeDivision - 1u));
	indices.emplace_back(iSouthPole);
}

void SphereMesh::CalculateNormals(std::vector<Vertex>& vertices) noexcept
{
	// A sphere's vertices are facing towards the outside from the centre. So, the normal
	// of a vertex would be vertex's position vector - the position vector of the centre.
	// Since the centre is at (0, 0, 0), the normal of a vertex would be its unit vector
	for (Vertex& vertex : vertices)
	{
		DirectX::XMVECTOR unitVector = DirectX::XMVector3Normalize(
			DirectX::XMLoadFloat3(&vertex.position)
		);

		DirectX::XMStoreFloat3(&vertex.normal, unitVector);
	}
}
