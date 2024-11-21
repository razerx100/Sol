#include <ranges>
#include <algorithm>
#include <cmath>
#include <MeshBoundImpl.hpp>

AxisAlignedBoundingBox GenerateAABB(const std::vector<Vertex>& vertices) noexcept
{
	AxisAlignedBoundingBox aabb{};

	DirectX::XMFLOAT4& pAxes = aabb.maxAxes;
	DirectX::XMFLOAT4& nAxes = aabb.minAxes;

	for (const auto& vertex : vertices)
	{
		pAxes.x = std::max(vertex.position.x, pAxes.x);
		pAxes.y = std::max(vertex.position.y, pAxes.y);
		pAxes.z = std::max(vertex.position.z, pAxes.z);

		nAxes.x = std::min(vertex.position.x, nAxes.x);
		nAxes.y = std::min(vertex.position.y, nAxes.y);
		nAxes.z = std::min(vertex.position.z, nAxes.z);
	}

	pAxes.w = 1.f;
	nAxes.w = 1.f;

	return aabb;
}

AxisAlignedBoundingBox GenerateAABB(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept {
	AxisAlignedBoundingBox aabb{};

	DirectX::XMFLOAT4& pAxes = aabb.maxAxes;
	DirectX::XMFLOAT4& nAxes = aabb.minAxes;

	const auto vertexEnd = static_cast<size_t>(meshlet.vertexOffset + meshlet.vertexCount);

	for (size_t index = meshlet.vertexOffset; index < vertexEnd; ++index)
	{
		const Vertex& vertex = vertices[vertexIndices[index]];

		pAxes.x = std::max(vertex.position.x, pAxes.x);
		pAxes.y = std::max(vertex.position.y, pAxes.y);
		pAxes.z = std::max(vertex.position.z, pAxes.z);

		nAxes.x = std::min(vertex.position.x, nAxes.x);
		nAxes.y = std::min(vertex.position.y, nAxes.y);
		nAxes.z = std::min(vertex.position.z, nAxes.z);
	}

	pAxes.w = 1.f;
	nAxes.w = 1.f;

	return aabb;
}

SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept
{
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	// This function would probably be used for a model and so its centre should be at the origin.
	// But just to be sure.
	AxisAlignedBoundingBox aabb = GenerateAABB(vertices);

	const XMVECTOR centre       = (XMLoadFloat4(&aabb.maxAxes) + XMLoadFloat4(&aabb.minAxes)) * 0.5;

	XMStoreFloat4(&sphereVolume.sphere, centre);

	float radius = 0.f;

	for (const auto& vertex : vertices)
	{
		XMVECTOR vertexV  = XMLoadFloat3(&vertex.position);
		XMVECTOR distance = XMVector3Length(centre - vertexV);

		radius = std::max(radius, XMVectorGetX(distance));
	}

	sphereVolume.sphere.w = radius;

	return sphereVolume;
}

SphereBoundingVolume GenerateSphereBV(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept {
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	// This function would be for meshlets and the origin would be the centre of the model but
	// not the meshlets.
	AxisAlignedBoundingBox aabb = GenerateAABB(vertices, vertexIndices, meshlet);

	const XMVECTOR centre       = (XMLoadFloat4(&aabb.maxAxes) + XMLoadFloat4(&aabb.minAxes)) * 0.5;

	XMStoreFloat4(&sphereVolume.sphere, centre);

	float radius = 0.f;

	const auto vertexEnd = static_cast<size_t>(meshlet.vertexOffset + meshlet.vertexCount);

	for (size_t index = meshlet.vertexOffset; index < vertexEnd; ++index)
	{
		const Vertex& vertex = vertices[vertexIndices[index]];

		XMVECTOR vertexV  = XMLoadFloat3(&vertex.position);
		XMVECTOR distance = XMVector3Length(centre - vertexV);

		radius = std::max(radius, XMVectorGetX(distance));
	}

	sphereVolume.sphere.w = radius;

	return sphereVolume;
}
