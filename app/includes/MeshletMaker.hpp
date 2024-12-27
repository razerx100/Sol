#ifndef MESHLET_MAKER_HPP_
#define MESHLET_MAKER_HPP_
#include <vector>
#include <unordered_map>
#include <SolMeshUtility.hpp>
#include <assimp/scene.h>

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
	[[nodiscard]]
	static bool IsInMap(
		const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::uint32_t vIndex
	) noexcept;
	[[nodiscard]]
	static std::uint32_t GetPrimIndex(
		std::uint32_t vIndex, std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::vector<std::uint32_t>& vertexIndices
	) noexcept;
	[[nodiscard]]
	static std::uint32_t GetExtraIndexCount(
		const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
	) noexcept;

	[[nodiscard]]
	size_t MakeMeshlet(const std::vector<std::uint32_t>& indices, size_t startingIndex) noexcept;
	[[nodiscard]]
	size_t MakeMeshlet(aiFace* faces, size_t faceCount, size_t startingIndex) noexcept;

private:
	static constexpr size_t s_meshletVertexLimit    = 64u;
	static constexpr size_t s_meshletPrimitiveLimit = 126u;

private:
	std::vector<std::uint32_t>  m_tempVertexIndices;
	std::vector<std::uint32_t>  m_tempPrimitiveIndices;
	std::vector<std::uint32_t>  m_vertexIndices;
	std::vector<std::uint32_t>  m_primitiveIndices;
	std::vector<MeshletDetails> m_meshletDetails;
};
#endif
