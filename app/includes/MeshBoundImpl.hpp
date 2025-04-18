#ifndef MESH_BOUND_IMPL_HPP_
#define MESH_BOUND_IMPL_HPP_
#include <MeshBundle.hpp>
#include <BoundingVolumes.hpp>
#include <ConversionUtilities.hpp>
#include <assimp/aabb.h>

struct AABBGenerator
{
	AABBGenerator()
		: m_positiveAxes{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f) },
		m_negativeAxes{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f) }
	{}

	void ProcessVertex(const DirectX::XMFLOAT3& position) noexcept;

	[[nodiscard]]
	AxisAlignedBoundingBox GenerateAABB() const noexcept;

	DirectX::XMVECTOR m_positiveAxes;
	DirectX::XMVECTOR m_negativeAxes;
};

struct SphereBVGenerator
{
	SphereBVGenerator()
		: m_centre{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f) }, m_radius{ 0.f }
	{}

	void SetCentre(const AxisAlignedBoundingBox& aabb) noexcept;

	void ProcessVertex(const DirectX::XMFLOAT3& position) noexcept;

	[[nodiscard]]
	SphereBoundingVolume GenerateBV() const noexcept;

	DirectX::XMVECTOR m_centre;
	float             m_radius;
};

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
