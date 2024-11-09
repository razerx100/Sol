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

SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept
{
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	const XMVECTOR centre = XMLoadFloat4(&sphereVolume.sphere);

	float radius = 0.f;

	for (const auto& vertex : vertices)
	{
		XMVECTOR vertexV  = XMLoadFloat3(&vertex.position);
		XMVECTOR distance = XMVector3Length(centre - vertexV);

		float currentDistance = 0.f;
		XMStoreFloat(&currentDistance, distance);

		radius = std::max(radius, currentDistance);
	}

	sphereVolume.sphere.w = radius;

	return sphereVolume;
}
