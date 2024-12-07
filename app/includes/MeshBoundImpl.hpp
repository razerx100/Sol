#ifndef MESH_BOUND_IMPL_HPP_
#define MESH_BOUND_IMPL_HPP_
#include <MeshBundle.hpp>
#include <BoundingVolumes.hpp>
#include <ConversionUtilities.hpp>
#include <assimp/aabb.h>

[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(const std::vector<DirectX::XMFLOAT3>& positions) noexcept;
[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
AxisAlignedBoundingBox GetAABB(const aiAABB& aiAABB) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(const std::vector<DirectX::XMFLOAT3>& positions) noexcept;

[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept;
[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(
	aiVector3D* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept;

[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(
	aiVector3D* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept;

[[nodiscard]]
ClusterNormalCone GenerateNormalCone(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const std::vector<std::uint32_t>& primIndices, const MeshletDetails& meshletDetails
) noexcept;
[[nodiscard]]
ClusterNormalCone GenerateNormalCone(
	aiVector3D* vertices, aiVector3D* normals, const std::vector<std::uint32_t>& vertexIndices,
	const std::vector<std::uint32_t>& primIndices, const MeshletDetails& meshletDetails
) noexcept;
#endif
