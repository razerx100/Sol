#ifndef MESH_BOUND_IMPL_HPP_
#define MESH_BOUND_IMPL_HPP_
#include <MeshBundle.hpp>
#include <BoundingVolumes.hpp>

[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(const std::vector<DirectX::XMFLOAT3>& positions) noexcept;
[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(const std::vector<DirectX::XMFLOAT3>& positions) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept;
[[nodiscard]]
ClusterNormalCone GenerateNormalCone(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const std::vector<std::uint32_t>& primIndices, const MeshletDetails& meshletDetails
) noexcept;
#endif
