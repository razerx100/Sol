#include <MeshBundleBase.hpp>
#include <ranges>
#include <algorithm>
#include <MeshBoundImpl.hpp>
#include <BoundingVolumes.hpp>

void MeshBundleBase::AddMesh(Mesh&& mesh, const MeshDetails& meshDetails) noexcept
{
	MeshDetails& meshDetailsN = m_bundleDetails.meshDetails.emplace_back(meshDetails);
	meshDetailsN.sphereBV     = GenerateSphereBV(mesh.vertices);

	std::ranges::move(mesh.bounds, std::back_inserter(m_bounds));
	std::ranges::move(mesh.indices, std::back_inserter(m_indices));
	std::ranges::move(mesh.vertices, std::back_inserter(m_vertices));
}

void MeshBundleBase::CalculateNormalsIndependentFaces(
	std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
) noexcept {
	for (size_t index = 0u; index < std::size(indices); index += 3)
	{
		Vertex& vertex1 = vertices[indices[index]];
		Vertex& vertex2 = vertices[indices[index + 1u]];
		Vertex& vertex3 = vertices[indices[index + 2u]];

		DirectX::XMFLOAT3 faceNormal = GetFaceNormal(
			vertex1.position, vertex2.position, vertex3.position
		);

		vertex1.normal = faceNormal;
		vertex2.normal = faceNormal;
		vertex3.normal = faceNormal;
	}
}

DirectX::XMFLOAT3 MeshBundleBase::GetFaceNormal(
	const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
	const DirectX::XMFLOAT3& position3
) noexcept {
	DirectX::XMVECTOR vertex1 = DirectX::XMLoadFloat3(&position1);
	DirectX::XMVECTOR vertex2 = DirectX::XMLoadFloat3(&position2);
	DirectX::XMVECTOR vertex3 = DirectX::XMLoadFloat3(&position3);

	// Edges should be calculated in this subtraction order for ClockWise triangle vertices
	// drawing order
	DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(vertex2, vertex1);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(vertex3, vertex1);

	DirectX::XMVECTOR faceNormalV = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(edge1, edge2)
	);
	DirectX::XMFLOAT3 faceNormal{};
	DirectX::XMStoreFloat3(&faceNormal, faceNormalV);

	return faceNormal;
}

void MeshBundleBase::SetUVToVertices(
	std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
) noexcept {
	for (size_t index = 0u; index < std::size(uvs); ++index)
		vertices[index].uv = uvs[index];
}

// Mesh Bundle MS
void MeshBundleBaseMS::AddMesh(
	Mesh&& mesh, MeshExtraForMesh&& extraMeshData, const MeshDetails& meshDetails
) noexcept {
	std::ranges::move(extraMeshData.meshlets, std::back_inserter(m_meshlets));
	std::ranges::move(extraMeshData.primIndices, std::back_inserter(m_primIndices));

	m_meshBase.AddMesh(std::move(mesh), meshDetails);
}

// Mesh Bundle General
MeshBundleGeneral& MeshBundleGeneral::AddMesh(Mesh&& mesh) noexcept
{
	MeshDetails meshDetails{ .sphereBV = GenerateSphereBV(mesh.vertices) };

	if (!s_modelTypeVS)
	{
		if (!m_bundleMS)
			m_bundleMS = std::make_unique<MeshBundleBaseMS>();

		MeshletMaker meshletMaker{};
		meshletMaker.GenerateMeshlets(mesh);

		meshletMaker.LoadVertexIndices(mesh.indices);

		MeshExtraForMesh extraMeshData = meshletMaker.GenerateExtraMeshData();

		meshDetails.elementCount       = static_cast<std::uint32_t>(std::size(extraMeshData.meshlets));
		meshDetails.elementOffset      = static_cast<std::uint32_t>(std::size(m_bundleMS->GetMeshlets()));

		m_bundleMS->AddMesh(std::move(mesh), std::move(extraMeshData), meshDetails);
	}
	else
	{
		if (!m_bundleVS)
			m_bundleVS = std::make_unique<MeshBundleBaseVS>();

		meshDetails.elementCount  = static_cast<std::uint32_t>(std::size(mesh.indices));
		meshDetails.elementOffset = static_cast<std::uint32_t>(std::size(m_bundleVS->GetIndices()));

		m_bundleVS->AddMesh(std::move(mesh), meshDetails);
	}

	return *this;
}

std::uint32_t MeshBundleGeneral::SetMeshBundle(Renderer& renderer)
{
	if (!s_modelTypeVS)
		return renderer.AddMeshBundle(std::move(m_bundleMS));
	else
		return renderer.AddMeshBundle(std::move(m_bundleVS));
}

// Meshlet Maker
MeshletMaker::MeshletMaker()
	: m_tempVertexIndices{}, m_tempPrimitiveIndices{}, m_vertexIndices{}, m_primitiveIndices{},
	m_meshlets{}
{
	m_tempVertexIndices.reserve(s_meshletVertexLimit);
	m_tempPrimitiveIndices.reserve(s_meshletPrimitiveLimit);
}

void MeshletMaker::GenerateMeshlets(const Mesh& mesh)
{
	const std::vector<std::uint32_t>& indices = mesh.indices;

	for (size_t start = 0u; start < std::size(indices);)
		start = MakeMeshlet(indices, start);
}

void MeshletMaker::LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices)
{
	vertexIndices = std::move(m_vertexIndices);
}

MeshExtraForMesh MeshletMaker::GenerateExtraMeshData() noexcept
{
	return MeshExtraForMesh
	{
		.primIndices = std::move(m_primitiveIndices),
		.meshlets    = std::move(m_meshlets)
	};
}

bool MeshletMaker::IsInMap(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::uint32_t vIndex
) noexcept {
	return vertexIndicesMap.find(vIndex) != std::end(vertexIndicesMap);
}

std::uint32_t MeshletMaker::GetPrimIndex(
	std::uint32_t vIndex, std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::vector<std::uint32_t>& vertexIndices
) noexcept {
	if (!IsInMap(vertexIndicesMap, vIndex))
	{
		vertexIndicesMap.emplace(
			vIndex, static_cast<std::uint32_t>(std::size(vertexIndices))
		);
		vertexIndices.emplace_back(vIndex);
	}

	return vertexIndicesMap[vIndex];
}

std::uint32_t MeshletMaker::GetExtraVertexCount(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
) noexcept {
	std::uint32_t extraVertexCount = 0u;

	if (!IsInMap(vertexIndicesMap, primIndex1))
		++extraVertexCount;

	if (!IsInMap(vertexIndicesMap, primIndex2))
		++extraVertexCount;

	if (!IsInMap(vertexIndicesMap, primIndex3))
		++extraVertexCount;

	return extraVertexCount;
}

size_t MeshletMaker::MakeMeshlet(const std::vector<std::uint32_t>& indices, size_t startingIndex) noexcept
{
	const size_t meshletIndexLimit = startingIndex + s_meshletPrimitiveLimit * 3u;

	// I would love to make this map into static or something but that isn't going to work. As least
	// in this approach. A meshlet should have 64 vertices and 126 primitives. The problem arises when
	// there is a huge mesh with million vertices, what if a triangle is formed by for example if the
	// 1st, 1M th, and the 999th vertices decide to make a triangle, it would be really hard to define
	// the vertex offset with the limit 64, if there is only a single instance of those vertex indices
	// So, making a new duplicate and adding it at the end would solve the issue. So, can't keep the map
	// from any previous meshlets. And since the unordered map is a linked_list, there is no point in
	// preserving it like a vector, as the allocation can't be reused. Maybe I can use the flatmap when it
	// is implemented on MSVC.
	std::unordered_map<std::uint32_t, std::uint32_t> vertexIndicesMap{};

	m_tempVertexIndices.clear();
	m_tempPrimitiveIndices.clear();

	size_t indexOffset                   = startingIndex;
	constexpr size_t triangleVertexCount = 3u;

	for (; indexOffset + 2u < std::size(indices) && indexOffset < meshletIndexLimit;
		indexOffset += triangleVertexCount)
	{
		const std::uint32_t vIndex1 = indices[indexOffset];
		const std::uint32_t vIndex2 = indices[indexOffset + 1u];
		const std::uint32_t vIndex3 = indices[indexOffset + 2u];

		const auto vertexCount = static_cast<std::uint32_t>(std::size(m_tempVertexIndices));

		const std::uint32_t couldBeVertexCount = vertexCount + GetExtraVertexCount(
			vertexIndicesMap, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVertexCount > s_meshletVertexLimit)
			break;

		// The prim indices are basically the local vertex indices. So, they shouldn't be bigger
		// than uint10.
		// And the vertexIndices are basically the unique vertex inidices. With the duplicates
		// removed. The prim indices index into the vertexIndices and then that index is used to
		// find the actual vertex.
		const std::uint32_t primIndexOne   = GetPrimIndex(vIndex1, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexTwo   = GetPrimIndex(vIndex2, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(vIndex3, vertexIndicesMap, m_tempVertexIndices);

		m_tempPrimitiveIndices.emplace_back(
			PrimitiveIndices{ .unpacked = { primIndexOne, primIndexTwo, primIndexThree } }.packed
		);
	}

	const auto vertexOffset    = static_cast<std::uint32_t>(std::size(m_vertexIndices));
	const auto primitiveOffset = static_cast<std::uint32_t>(std::size(m_primitiveIndices));

	m_meshlets.emplace_back(Meshlet{
		.vertexCount     = static_cast<std::uint32_t>(std::size(m_tempVertexIndices)),
		.vertexOffset    = static_cast<std::uint32_t>(vertexOffset),
		.primitiveCount  = static_cast<std::uint32_t>(std::size(m_tempPrimitiveIndices)),
		.primitiveOffset = static_cast<std::uint32_t>(primitiveOffset)
	});

	std::ranges::move(m_tempVertexIndices, std::back_inserter(m_vertexIndices));
	std::ranges::move(m_tempPrimitiveIndices, std::back_inserter(m_primitiveIndices));

	return indexOffset;
}
