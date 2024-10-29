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

struct MeshBound
{
	DirectX::XMFLOAT3 position;
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
	std::uint32_t        elementCount;
	std::uint32_t        elementOffset;
	// We can only bind a SphereBV with a mesh, as it isn't affected by the rotation.
	// And so doesn't need to be updated.
	SphereBoundingVolume sphereBV;
};

struct MeshBundleDetails
{
	std::vector<MeshDetails> meshDetails;
};

class MeshBundle
{
public:
	virtual ~MeshBundle() = default;

	[[nodiscard]]
	virtual const std::vector<MeshBound>& GetBounds() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<Vertex>& GetVertices() const noexcept = 0;
	[[nodiscard]]
	virtual MeshBundleDetails&& GetBundleDetails() noexcept = 0;
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
	virtual const std::vector<Meshlet>& GetMeshlets() const noexcept = 0;
};
#endif
