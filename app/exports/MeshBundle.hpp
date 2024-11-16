#ifndef MESH_BUNDLE_SOL_HPP_
#define MESH_BUNDLE_SOL_HPP_
#include <cstdint>
#include <vector>
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
	std::uint32_t vertexCount;
	std::uint32_t vertexOffset;
	std::uint32_t primitiveCount;
	std::uint32_t primitiveOffset;
};

struct MeshDetails
{
	std::uint32_t          elementCount;
	std::uint32_t          elementOffset;
	AxisAlignedBoundingBox aabb;
};

struct MeshBundleDetails
{
	std::vector<MeshDetails> meshDetails;
};

struct MeshletDetails
{
	Meshlet              meshlet;
	SphereBoundingVolume sphereB;
};

class MeshBundle
{
public:
	virtual ~MeshBundle() = default;

	[[nodiscard]]
	virtual const std::vector<Vertex>& GetVertices() const noexcept = 0;
	[[nodiscard]]
	virtual MeshBundleDetails&& GetBundleDetails() noexcept = 0;
	[[nodiscard]]
	virtual const MeshBundleDetails& GetBundleDetails() const noexcept = 0;
};

class MeshBundleVS : public MeshBundle
{
public:
	virtual ~MeshBundleVS() = default;

	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetIndices() const noexcept = 0;
};

class MeshBundleMS : public MeshBundle
{
public:
	virtual ~MeshBundleMS() = default;

	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetVertexIndices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetPrimIndices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<MeshletDetails>& GetMeshletDetails() const noexcept = 0;
};
#endif
