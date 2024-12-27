#include <MeshletMaker.hpp>

// Meshlet Maker
MeshletMaker::MeshletMaker()
	: m_tempVertexIndices{}, m_tempPrimitiveIndices{}, m_vertexIndices{}, m_primitiveIndices{},
	m_meshletDetails{}
{
	m_tempVertexIndices.reserve(s_meshletVertexLimit);
	m_tempPrimitiveIndices.reserve(s_meshletPrimitiveLimit);
}

void MeshletMaker::GenerateMeshlets(const Mesh& mesh) noexcept
{
	const std::vector<std::uint32_t>& indices = mesh.indices;

	for (size_t start = 0u; start < std::size(indices);)
		start = MakeMeshlet(indices, start);
}

void MeshletMaker::GenerateMeshlets(aiMesh const* mesh) noexcept
{
	size_t faceCount = mesh->mNumFaces;
	aiFace* faces    = mesh->mFaces;

	for (size_t start = 0u; start < faceCount;)
		start = MakeMeshlet(faces, faceCount, start);
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

std::uint32_t MeshletMaker::GetExtraIndexCount(
	const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
	std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
) noexcept {
	std::uint32_t extraIndexCount = 0u;

	if (!IsInMap(vertexIndicesMap, primIndex1))
		++extraIndexCount;

	if (!IsInMap(vertexIndicesMap, primIndex2))
		++extraIndexCount;

	if (!IsInMap(vertexIndicesMap, primIndex3))
		++extraIndexCount;

	return extraIndexCount;
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

		const auto vIndexCount = static_cast<std::uint32_t>(std::size(m_tempVertexIndices));

		const std::uint32_t couldBeVIndexCount = vIndexCount + GetExtraIndexCount(
			vertexIndicesMap, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVIndexCount > s_meshletVertexLimit)
			break;

		// The prim indices are the local vertex indices. As in, they are local to the meshlet.
		// We are assuming each primitive would be a triangle. Each prim index actually has the
		// three local vertex indices which create that triangle. So, actually the primIndices
		// contain the indices of the primitive/triangles for that meshlet.

		// And the vertexIndices are the unique vertex indices. With the duplicates
		// removed. The indices of the primitives index into the vertexIndices and then that index
		// is used to find the actual vertex.
		const std::uint32_t primIndexOne   = GetPrimIndex(vIndex1, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexTwo   = GetPrimIndex(vIndex2, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(vIndex3, vertexIndicesMap, m_tempVertexIndices);

		PrimitiveIndicesUnpacked unpackedPrim
		{
			.firstIndex  = primIndexOne,
			.secondIndex = primIndexTwo,
			.thirdIndex  = primIndexThree
		};

		std::uint32_t packedPrim = 0u;

		memcpy(&packedPrim, &unpackedPrim, sizeof(std::uint32_t));

		m_tempPrimitiveIndices.emplace_back(packedPrim);
	}

	const auto vertexOffset    = static_cast<std::uint32_t>(std::size(m_vertexIndices));
	const auto primitiveOffset = static_cast<std::uint32_t>(std::size(m_primitiveIndices));

	m_meshletDetails.emplace_back(
		MeshletDetails
		{
			.meshlet = Meshlet
			{
				.indexCount      = static_cast<std::uint32_t>(std::size(m_tempVertexIndices)),
				.indexOffset     = static_cast<std::uint32_t>(vertexOffset),
				.primitiveCount  = static_cast<std::uint32_t>(std::size(m_tempPrimitiveIndices)),
				.primitiveOffset = static_cast<std::uint32_t>(primitiveOffset)
			}
		}
	);

	MemcpyIntoVector(m_vertexIndices, m_tempVertexIndices);
	MemcpyIntoVector(m_primitiveIndices, m_tempPrimitiveIndices);

	return indexOffset;
}

size_t MeshletMaker::MakeMeshlet(aiFace* faces, size_t faceCount, size_t startingIndex) noexcept
{
	const size_t meshletIndexLimit = startingIndex + s_meshletPrimitiveLimit;

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

	size_t indexOffset = startingIndex;

	for (;indexOffset < faceCount && indexOffset < meshletIndexLimit; ++indexOffset)
	{
		const aiFace& face = faces[indexOffset];

		// Each face should be a triangle.
		const std::uint32_t vIndex1 = face.mIndices[0];
		const std::uint32_t vIndex2 = face.mIndices[1];
		const std::uint32_t vIndex3 = face.mIndices[2];

		const auto vIndexCount = static_cast<std::uint32_t>(std::size(m_tempVertexIndices));

		const std::uint32_t couldBeVIndexCount = vIndexCount + GetExtraIndexCount(
			vertexIndicesMap, vIndex1, vIndex2, vIndex3
		);

		if (couldBeVIndexCount > s_meshletVertexLimit)
			break;

		// The prim indices are the local vertex indices. As in, they are local to the meshlet.
		// We are assuming each primitive would be a triangle. Each prim index actually has the
		// three local vertex indices which create that triangle. So, actually the primIndices
		// contain the indices of the primitive/triangles for that meshlet.

		// And the vertexIndices are the unique vertex indices. With the duplicates
		// removed. The indices of the primitives index into the vertexIndices and then that index
		// is used to find the actual vertex.
		const std::uint32_t primIndexOne   = GetPrimIndex(vIndex1, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexTwo   = GetPrimIndex(vIndex2, vertexIndicesMap, m_tempVertexIndices);
		const std::uint32_t primIndexThree = GetPrimIndex(vIndex3, vertexIndicesMap, m_tempVertexIndices);

		PrimitiveIndicesUnpacked unpackedPrim
		{
			.firstIndex  = primIndexOne,
			.secondIndex = primIndexTwo,
			.thirdIndex  = primIndexThree
		};

		std::uint32_t packedPrim = 0u;

		memcpy(&packedPrim, &unpackedPrim, sizeof(std::uint32_t));

		m_tempPrimitiveIndices.emplace_back(packedPrim);
	}

	const auto localVertexOffset = static_cast<std::uint32_t>(std::size(m_vertexIndices));
	const auto primitiveOffset   = static_cast<std::uint32_t>(std::size(m_primitiveIndices));

	m_meshletDetails.emplace_back(
		MeshletDetails
		{
			.meshlet = Meshlet
			{
				.indexCount      = static_cast<std::uint32_t>(std::size(m_tempVertexIndices)),
				.indexOffset     = static_cast<std::uint32_t>(localVertexOffset),
				.primitiveCount  = static_cast<std::uint32_t>(std::size(m_tempPrimitiveIndices)),
				.primitiveOffset = static_cast<std::uint32_t>(primitiveOffset)
			}
		}
	);

	MemcpyIntoVector(m_vertexIndices, m_tempVertexIndices);
	MemcpyIntoVector(m_primitiveIndices, m_tempPrimitiveIndices);

	return indexOffset;
}

MeshletMaker::PrimitiveIndicesUnpacked MeshletMaker::UnpackPrim(std::uint32_t packedIndices) noexcept
{
	PrimitiveIndicesUnpacked unpackedIndices{ 0u, 0u, 0u };

	memcpy(&unpackedIndices, &packedIndices, sizeof(std::uint32_t));

	return unpackedIndices;
}

