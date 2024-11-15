#ifndef MESH_BOUND_IMPL_HPP_
#define MESH_BOUND_IMPL_HPP_
#include <MeshBundle.hpp>
#include <BoundingVolumes.hpp>

[[nodiscard]]
AxisAlignedBoundingBox GenerateAABB(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept;
[[nodiscard]]
SphereBoundingVolume GenerateSphereBV(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept;
#endif
