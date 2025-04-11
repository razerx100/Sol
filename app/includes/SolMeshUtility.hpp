#ifndef SOL_MESH_UTILITY_HPP_
#define SOL_MESH_UTILITY_HPP_
#include <utility>
#include <cstring>
#include <vector>
#include <MeshBundle.hpp>
#include <Model.hpp>

struct Mesh
{
	std::vector<Vertex>        vertices;
	std::vector<std::uint32_t> indices;
};

struct MeshExtraForMesh
{
	std::vector<std::uint32_t>  primIndices;
	std::vector<MeshletDetails> meshletDetails;
};

struct MeshBundleTemporaryData
{
	std::vector<Vertex>         vertices;
	std::vector<std::uint32_t>  indices;
	std::vector<std::uint32_t>  primIndices;
	std::vector<MeshletDetails> meshletDetails;
	MeshBundleTemporaryDetails  bundleDetails;
};

struct MeshChildrenData
{
	std::uint32_t count;
	std::uint32_t startingIndex;
};

struct MeshNodeData
{
	std::uint32_t    modelIndex   = std::numeric_limits<std::uint32_t>::max();
	std::uint32_t    meshIndex    = std::numeric_limits<std::uint32_t>::max();
	MeshChildrenData childrenData;

	[[nodiscard]]
	bool HasMesh() const noexcept { return meshIndex != std::numeric_limits<std::uint32_t>::max(); }
};

struct MeshTextureDetails
{
	// The indices of the resources.
	std::uint32_t textureIndex     = std::numeric_limits<std::uint32_t>::max();
	// The indices of the bound texures, which can be used.
	std::uint32_t textureBindIndex = std::numeric_limits<std::uint32_t>::max();
	UVInfo        uvInfo{};
};

struct MeshMaterialDetails
{
	std::vector<MeshTextureDetails> diffuseTextures;
	std::vector<MeshTextureDetails> specularTextures;
	std::uint32_t                   materialIndex;
	std::uint32_t                   pipelineIndex;
};

struct MeshPermanentDetails
{
	DirectX::XMMATRIX   worldMatrix;
	MeshMaterialDetails materialDetails;
};

template<typename T>
static void MemcpyIntoVector(std::vector<T>& dst, const std::vector<T>& src) noexcept
{
	const size_t srcElementCount = std::size(src);
	const size_t dstElementCount = std::size(dst);

	const size_t srcDataSize  = sizeof(T) * srcElementCount;
	// Since std::data returns T*, the start would be the old count.
	const size_t dstDataStart = dstElementCount;

	dst.resize(srcElementCount + dstElementCount);

	memcpy(std::data(dst) + dstDataStart, std::data(src), srcDataSize);
}

void CopyAndOffsetIndices(
	std::vector<std::uint32_t>& dst, const std::vector<std::uint32_t>& src, std::uint32_t offset
) noexcept;

void SetDefaultTextureDetails(std::uint32_t textureIndex, std::uint32_t bindingIndex) noexcept;

[[nodiscard]]
const MeshTextureDetails& GetDefaultTextureDetails() noexcept;
#endif
