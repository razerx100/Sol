#include <MeshletMaker.hpp>

// Meshlet Generator
MeshletGenerator::MeshletGenerator(
	std::vector<std::uint32_t>& vertexIndices,
	std::vector<std::uint32_t>& primitiveIndices
) : m_vertexIndices{ vertexIndices }, m_vertexIndexOffset{ std::size(vertexIndices) },
	m_primitiveIndices{ primitiveIndices },
	m_primitiveIndexOffset{ std::size(primitiveIndices) }, m_primitiveIndexMap{}
{
	m_vertexIndices.reserve(m_vertexIndexOffset + s_meshletVertexLimit);
	m_primitiveIndices.reserve(m_primitiveIndexOffset + s_meshletPrimitiveLimit);
}

size_t MeshletGenerator::GetMeshletVertexIndexCount() const noexcept
{
	// Since there will be the previous vertex indices from other meshlets.
	return std::size(m_vertexIndices) - m_vertexIndexOffset;
}

size_t MeshletGenerator::GetMeshletPrimitiveIndexCount() const noexcept
{
	// Since there will be the previous primtive indices from other meshlets.
	return std::size(m_primitiveIndices) - m_primitiveIndexOffset;
}

bool MeshletGenerator::ProcessPrimitive(const PrimTriangle& primitive)
{
	const size_t vertexIndexCount    = GetMeshletVertexIndexCount();
	const size_t primitiveIndexCount = GetMeshletPrimitiveIndexCount();

	const size_t addableVertexIndexCount = GetAddableVertexIndexCount(primitive);

	const size_t newVertexIndexCount    = vertexIndexCount + addableVertexIndexCount;
	const size_t newPrimitiveIndexCount = primitiveIndexCount + 1u;

	const bool hasMeshletLimitReached
		= newVertexIndexCount > s_meshletVertexLimit
		|| newPrimitiveIndexCount > s_meshletPrimitiveLimit;

	if (hasMeshletLimitReached)
		return false;

	// The prim indices are the local vertex indices. As in, they are local to the meshlet.
	// We are assuming each primitive would be a triangle. Each prim index actually has the
	// three local vertex indices which create that triangle. So, actually the primIndices
	// contain the indices of the primitive/triangles for that meshlet.

	// And the vertex indices are the unique vertex indices. With the duplicates
	// removed. The indices of the primitives index into the vertexIndices and then that index
	// is used to find the actual vertex.
	const std::uint32_t primIndexOne   = GetOrAddPrimitiveIndex(primitive.vertex0);
	const std::uint32_t primIndexTwo   = GetOrAddPrimitiveIndex(primitive.vertex1);
	const std::uint32_t primIndexThree = GetOrAddPrimitiveIndex(primitive.vertex2);

	PrimitiveIndicesUnpacked unpackedPrim
	{
		.firstIndex  = primIndexOne,
		.secondIndex = primIndexTwo,
		.thirdIndex  = primIndexThree
	};

	std::uint32_t packedPrim = 0u;

	static_assert(
		sizeof(PrimitiveIndicesUnpacked) == sizeof(std::uint32_t),
		"The size of Primitive Indices Unpacked and U32 should be the same."
	);

	memcpy(&packedPrim, &unpackedPrim, sizeof(std::uint32_t));

	m_primitiveIndices.emplace_back(packedPrim);

	return true;
}

Meshlet MeshletGenerator::GenerateMeshlet() const noexcept
{
	const auto meshletVertexIndexCount = static_cast<std::uint32_t>(GetMeshletVertexIndexCount());
	const auto meshletPrimitiveIndexCount
		= static_cast<std::uint32_t>(GetMeshletPrimitiveIndexCount());

	return Meshlet
		{
			.indexCount      = meshletVertexIndexCount,
			.indexOffset     = static_cast<std::uint32_t>(m_vertexIndexOffset),
			.primitiveCount  = meshletPrimitiveIndexCount,
			.primitiveOffset = static_cast<std::uint32_t>(m_primitiveIndexOffset)
		};
}

bool MeshletGenerator::IsInMap(std::uint32_t vertexIndex) const noexcept
{
	return m_primitiveIndexMap.find(vertexIndex) != std::end(m_primitiveIndexMap);
}

size_t MeshletGenerator::GetAddableVertexIndexCount(
	const PrimTriangle& primitive
) const noexcept {
	size_t addableIndexCount = 0u;

	if (IsInMap(primitive.vertex0))
		++addableIndexCount;

	if (IsInMap(primitive.vertex1))
		++addableIndexCount;

	if (IsInMap(primitive.vertex2))
		++addableIndexCount;

	return addableIndexCount;
}

std::uint32_t MeshletGenerator::GetOrAddPrimitiveIndex(std::uint32_t vertexIndex) noexcept
{
	if (!IsInMap(vertexIndex))
	{
		m_primitiveIndexMap.emplace(
			vertexIndex, static_cast<std::uint32_t>(GetMeshletVertexIndexCount())
		);
		m_vertexIndices.emplace_back(vertexIndex);
	}

	return m_primitiveIndexMap[vertexIndex];
}

MeshletGenerator::PrimitiveIndicesUnpacked MeshletGenerator::UnpackPrim(
	std::uint32_t packedIndices
) noexcept {
	PrimitiveIndicesUnpacked unpackedIndices{ 0u, 0u, 0u };

	memcpy(&unpackedIndices, &packedIndices, sizeof(std::uint32_t));

	return unpackedIndices;
}

// Meshlet Maker
MeshletMaker::MeshletMaker() : m_vertexIndices{}, m_primitiveIndices{}, m_meshletDetails{} {}

void MeshletMaker::GenerateMeshlets(const Mesh& mesh) noexcept
{
	MakeMeshlets(mesh.indices);
}

void MeshletMaker::GenerateMeshlets(aiMesh const* mesh) noexcept
{
	MakeMeshlets(mesh->mFaces, mesh->mNumFaces);
}

void MeshletMaker::LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices) noexcept
{
	vertexIndices = std::move(m_vertexIndices);
}

MeshExtraForMesh MeshletMaker::GenerateExtraMeshData() noexcept
{
	return MeshExtraForMesh
	{
		.primIndices    = std::move(m_primitiveIndices),
		.meshletDetails = std::move(m_meshletDetails)
	};
}

void MeshletMaker::MakeMeshlets(const std::vector<std::uint32_t>& indices) noexcept
{
	MeshletGenerator meshletGen{ m_vertexIndices, m_primitiveIndices };

	const size_t indexCount = std::size(indices);

	for (size_t index = 0u; index + 2u < indexCount; index += 3u)
	{
		MeshletGenerator::PrimTriangle triangle
		{
			.vertex0 = indices[index + 0u],
			.vertex1 = indices[index + 1u],
			.vertex2 = indices[index + 2u]
		};

		const bool isMeshletProcessed = meshletGen.ProcessPrimitive(triangle);

		if (isMeshletProcessed)
		{
			m_meshletDetails.emplace_back(
				MeshletDetails
				{
					.meshlet = meshletGen.GenerateMeshlet()
				}
			);

			meshletGen = MeshletGenerator{ m_vertexIndices, m_primitiveIndices };
		}
	}
}

void MeshletMaker::MakeMeshlets(aiFace* faces, size_t faceCount) noexcept
{
	MeshletGenerator meshletGen{ m_vertexIndices, m_primitiveIndices };

	for (size_t index = 0u; index < faceCount; ++index)
	{
		const aiFace& face = faces[index];

		MeshletGenerator::PrimTriangle triangle
		{
			.vertex0 = face.mIndices[0],
			.vertex1 = face.mIndices[1],
			.vertex2 = face.mIndices[2]
		};

		const bool isMeshletProcessed = meshletGen.ProcessPrimitive(triangle);

		if (isMeshletProcessed)
		{
			m_meshletDetails.emplace_back(
				MeshletDetails
				{
					.meshlet = meshletGen.GenerateMeshlet()
				}
			);

			meshletGen = MeshletGenerator{ m_vertexIndices, m_primitiveIndices };
		}
	}
}

MeshletMaker::PrimitiveIndicesUnpacked MeshletMaker::UnpackPrim(
	std::uint32_t packedIndices
) noexcept {
	PrimitiveIndicesUnpacked unpackedIndices{ 0u, 0u, 0u };

	memcpy(&unpackedIndices, &packedIndices, sizeof(std::uint32_t));

	return unpackedIndices;
}

