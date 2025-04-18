#ifndef MESHLET_MAKER_HPP_
#define MESHLET_MAKER_HPP_
#include <vector>
#include <unordered_map>
#include <SolMeshUtility.hpp>
#include <assimp/scene.h>

class MeshletGenerator
{
public:
	struct PrimTriangle
	{
		std::uint32_t vertex0;
		std::uint32_t vertex1;
		std::uint32_t vertex2;
	};

	struct PrimitiveIndicesUnpacked
	{
		std::uint32_t firstIndex  : 10u;
		std::uint32_t secondIndex : 10u;
		std::uint32_t thirdIndex  : 10u;
	};

	static constexpr size_t s_meshletVertexLimit    = 64u;
	static constexpr size_t s_meshletPrimitiveLimit = 126u;

public:
	MeshletGenerator(
		std::vector<std::uint32_t>& vertexIndices,
		std::vector<std::uint32_t>& primitiveIndices
	);

	// Returns false if either the vertex or primitive limit is reached. Should create
	// the meshlet at that point and start processing a new meshlet.
	[[nodiscard]]
	bool ProcessPrimitive(const PrimTriangle& primitive);

	[[nodiscard]]
	Meshlet GenerateMeshlet() const noexcept;

	[[nodiscard]]
	static PrimitiveIndicesUnpacked UnpackPrim(std::uint32_t packedIndices) noexcept;

private:
	[[nodiscard]]
	bool IsInMap(std::uint32_t vertexIndex) const noexcept;
	[[nodiscard]]
	size_t GetAddableVertexIndexCount(const PrimTriangle& primitive) const noexcept;

	[[nodiscard]]
	std::uint32_t GetOrAddPrimitiveIndex(std::uint32_t vertexIndex) noexcept;

	[[nodiscard]]
	size_t GetMeshletVertexIndexCount() const noexcept;
	[[nodiscard]]
	size_t GetMeshletPrimitiveIndexCount() const noexcept;

private:
	std::vector<std::uint32_t>&                      m_vertexIndices;
	size_t                                           m_vertexIndexOffset;
	std::vector<std::uint32_t>&                      m_primitiveIndices;
	size_t                                           m_primitiveIndexOffset;
	// I might want to replace this with a flat_map one day.
	std::unordered_map<std::uint32_t, std::uint32_t> m_primitiveIndexMap;

public:
	MeshletGenerator(const MeshletGenerator&) = delete;
	MeshletGenerator& operator=(const MeshletGenerator&) = delete;

	MeshletGenerator(MeshletGenerator&& other) noexcept
		: m_vertexIndices{ other.m_vertexIndices },
		m_vertexIndexOffset{ other.m_vertexIndexOffset },
		m_primitiveIndices{ other.m_primitiveIndices },
		m_primitiveIndexOffset{ other.m_primitiveIndexOffset },
		m_primitiveIndexMap{ std::move(other.m_primitiveIndexMap) }
	{}
	MeshletGenerator& operator=(MeshletGenerator&& other) noexcept
	{
		m_vertexIndices        = other.m_vertexIndices;
		m_vertexIndexOffset    = other.m_vertexIndexOffset;
		m_primitiveIndices     = other.m_primitiveIndices;
		m_primitiveIndexOffset = other.m_primitiveIndexOffset;
		m_primitiveIndexMap    = std::move(other.m_primitiveIndexMap);

		return *this;
	}
};

class MeshletMaker
{
public:
	struct PrimitiveIndicesUnpacked
	{
		std::uint32_t firstIndex  : 10u;
		std::uint32_t secondIndex : 10u;
		std::uint32_t thirdIndex  : 10u;
	};

public:
	MeshletMaker();

	void GenerateMeshlets(const Mesh& mesh) noexcept;
	void GenerateMeshlets(aiMesh const* mesh) noexcept;

	void LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices) noexcept;

	[[nodiscard]]
	// It is not const, because it will move the data.
	MeshExtraForMesh GenerateExtraMeshData() noexcept;

	[[nodiscard]]
	static PrimitiveIndicesUnpacked UnpackPrim(std::uint32_t packedIndices) noexcept;

private:
	void MakeMeshlets(const std::vector<std::uint32_t>& indices) noexcept;
	void MakeMeshlets(aiFace* faces, size_t faceCount) noexcept;

private:
	std::vector<std::uint32_t>  m_vertexIndices;
	std::vector<std::uint32_t>  m_primitiveIndices;
	std::vector<MeshletDetails> m_meshletDetails;
};
#endif
