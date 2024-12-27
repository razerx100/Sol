#ifndef SOL_MESH_UTILITY_HPP_
#define SOL_MESH_UTILITY_HPP_
#include <utility>
#include <cstring>
#include <vector>
#include <MeshBundle.hpp>

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

struct MeshPermanentDetails
{
	DirectX::XMMATRIX worldMatrix;
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
#endif
