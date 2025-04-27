#ifndef MESH_BUNDLE_SOL_HPP_
#define MESH_BUNDLE_SOL_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <BoundingVolumes.hpp>

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct Meshlet
{
	std::uint32_t indexCount;
	std::uint32_t indexOffset;
	std::uint32_t primitiveCount;
	std::uint32_t primitiveOffset;
};

struct MeshTemporaryDetailsVS
{
	std::uint32_t          indexCount;
	std::uint32_t          indexOffset;
	// Can't really offset the indices in the shader on a Vertex Shader,
	// as the Input Assembler will handle that. So, will have to offset it
	// while generating the data.
	AxisAlignedBoundingBox aabb;
};

struct MeshTemporaryDetailsMS
{
	std::uint32_t          meshletCount;
	std::uint32_t          meshletOffset;
	std::uint32_t          indexOffset;
	std::uint32_t          primitiveOffset;
	std::uint32_t          vertexOffset;
	AxisAlignedBoundingBox aabb;
};

struct MeshBundleTemporaryDetails
{
	std::vector<MeshTemporaryDetailsVS> meshTemporaryDetailsVS;
	std::vector<MeshTemporaryDetailsMS> meshTemporaryDetailsMS;
};

struct MeshletDetails
{
	Meshlet              meshlet;
	SphereBoundingVolume sphereB;
	ClusterNormalCone    coneNormal;
};

struct MeshBundleTemporaryData
{
	std::vector<Vertex>         vertices;
	std::vector<std::uint32_t>  indices;
	std::vector<std::uint32_t>  primIndices;
	std::vector<MeshletDetails> meshletDetails;
	MeshBundleTemporaryDetails  bundleDetails;
};
#endif
