#include <ranges>
#include <algorithm>
#include <cmath>
#include <MeshBoundImpl.hpp>
#include <MeshBundleBase.hpp>
#include <DirectXPackedVector.h>

AxisAlignedBoundingBox GetAABB(const aiAABB& aiAABB) noexcept
{
	AxisAlignedBoundingBox aabb
	{
		.maxAxes = DirectX::XMFLOAT4{ aiAABB.mMax.x, aiAABB.mMax.y, aiAABB.mMax.z, 1.f },
		.minAxes = DirectX::XMFLOAT4{ aiAABB.mMin.x, aiAABB.mMin.y, aiAABB.mMin.z, 1.f }
	};

	return aabb;
}

AxisAlignedBoundingBox GenerateAABB(const std::vector<Vertex>& vertices) noexcept
{
	using namespace DirectX;

	AxisAlignedBoundingBox aabb{};

	XMVECTOR pAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR nAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	for (const auto& vertex : vertices)
	{
		XMVECTOR vertexPosition = XMLoadFloat3(&vertex.position);

		pAxesV = XMVectorMax(vertexPosition, pAxesV);
		nAxesV = XMVectorMin(vertexPosition, nAxesV);
	}

	XMFLOAT4& pAxes = aabb.maxAxes;
	XMFLOAT4& nAxes = aabb.minAxes;

	XMStoreFloat4(&pAxes, pAxesV);
	XMStoreFloat4(&nAxes, nAxesV);

	pAxes.w = 1.f;
	nAxes.w = 1.f;

	return aabb;
}

AxisAlignedBoundingBox GenerateAABB(const std::vector<DirectX::XMFLOAT3>& positions) noexcept
{
	using namespace DirectX;

	AxisAlignedBoundingBox aabb{};

	XMVECTOR pAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR nAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	for (const auto& position : positions)
	{
		XMVECTOR vertexPosition = XMLoadFloat3(&position);

		pAxesV = XMVectorMax(vertexPosition, pAxesV);
		nAxesV = XMVectorMin(vertexPosition, nAxesV);
	}

	XMFLOAT4& pAxes = aabb.maxAxes;
	XMFLOAT4& nAxes = aabb.minAxes;

	XMStoreFloat4(&pAxes, pAxesV);
	XMStoreFloat4(&nAxes, nAxesV);

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

SphereBoundingVolume GenerateSphereBV(const std::vector<DirectX::XMFLOAT3>& positions) noexcept
{
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	// This function would probably be used for a model and so its centre should be at the origin.
	// But just to be sure.
	AxisAlignedBoundingBox aabb = GenerateAABB(positions);

	const XMVECTOR centre       = (XMLoadFloat4(&aabb.maxAxes) + XMLoadFloat4(&aabb.minAxes)) * 0.5;

	XMStoreFloat4(&sphereVolume.sphere, centre);

	float radius = 0.f;

	for (const auto& position : positions)
	{
		XMVECTOR vertexV  = XMLoadFloat3(&position);
		XMVECTOR distance = XMVector3Length(centre - vertexV);

		radius = std::max(radius, XMVectorGetX(distance));
	}

	sphereVolume.sphere.w = radius;

	return sphereVolume;
}

// Vertex component getters.
static DirectX::XMFLOAT3 GetPosition(const Vertex& vertex) noexcept
{
	return vertex.position;
}

static DirectX::XMFLOAT3 GetPosition(const aiVector3D& aiVertex) noexcept
{
	return GetXMFloat3(aiVertex);
}

// AABB for meshlets
template<typename Vertex_t>
[[nodiscard]]
static AxisAlignedBoundingBox _generateAABB(
	Vertex_t* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept {
	using namespace DirectX;

	AxisAlignedBoundingBox aabb{};

	XMVECTOR pAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR nAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	const auto vertexEnd = static_cast<size_t>(meshlet.vertexOffset + meshlet.vertexCount);

	for (size_t index = meshlet.vertexOffset; index < vertexEnd; ++index)
	{
		XMFLOAT3 vertexPositionF = GetPosition(vertices[vertexIndices[index]]);

		XMVECTOR vertexPosition = XMLoadFloat3(&vertexPositionF);

		pAxesV = XMVectorMax(vertexPosition, pAxesV);
		nAxesV = XMVectorMin(vertexPosition, nAxesV);
	}

	XMFLOAT4& pAxes = aabb.maxAxes;
	XMFLOAT4& nAxes = aabb.minAxes;

	XMStoreFloat4(&pAxes, pAxesV);
	XMStoreFloat4(&nAxes, nAxesV);

	pAxes.w = 1.f;
	nAxes.w = 1.f;

	return aabb;
}

AxisAlignedBoundingBox GenerateAABB(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const Meshlet& meshlet
) noexcept {
	return _generateAABB(std::data(vertices), vertexIndices, meshlet);
}

AxisAlignedBoundingBox GenerateAABB(
	aiVector3D* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept {
	return _generateAABB(vertices, vertexIndices, meshlet);
}

// SphereBV for meshlets
template<typename Vertex_t>
[[nodiscard]]
static SphereBoundingVolume _generateSphereBV(
	Vertex_t* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept {
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	// This function would be for meshlets and the origin would be the centre of the model but
	// not the meshlets.
	AxisAlignedBoundingBox aabb = _generateAABB(vertices, vertexIndices, meshlet);

	const XMVECTOR centre       = (XMLoadFloat4(&aabb.maxAxes) + XMLoadFloat4(&aabb.minAxes)) * 0.5;

	XMStoreFloat4(&sphereVolume.sphere, centre);

	float radius = 0.f;

	const auto vertexEnd = static_cast<size_t>(meshlet.vertexOffset + meshlet.vertexCount);

	for (size_t index = meshlet.vertexOffset; index < vertexEnd; ++index)
	{
		XMFLOAT3 vertexPositionF = GetPosition(vertices[vertexIndices[index]]);

		XMVECTOR vertexV  = XMLoadFloat3(&vertexPositionF);
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
	return _generateSphereBV(std::data(vertices), vertexIndices, meshlet);
}

SphereBoundingVolume GenerateSphereBV(
	aiVector3D* vertices, const std::vector<std::uint32_t>& vertexIndices, const Meshlet& meshlet
) noexcept {
	return _generateSphereBV(vertices, vertexIndices, meshlet);
}

// Cone normal for meshlets.
ClusterNormalCone GenerateNormalCone(
	const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& vertexIndices,
	const std::vector<std::uint32_t>& primIndices, const MeshletDetails& meshletDetails
) noexcept {
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	const Meshlet& meshlet  = meshletDetails.meshlet;

	const auto vertexOffset = static_cast<size_t>(meshlet.vertexOffset);
	const auto primOffset   = static_cast<size_t>(meshlet.primitiveOffset);
	const auto primCount    = static_cast<size_t>(meshlet.primitiveCount);

	XMVECTOR positiveAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR negativeAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		const Vertex& primVertex1 = vertices[vertexIndices[vertexOffset + unpackedIndices.firstIndex]];

		// All of the vertex normals of a triangle should point towards the same direction.

		{
			// Calculate the AABB of the normals, so we can calculate the centre. Not using the other
			// functions, as the parameters would need to be different.
			XMVECTOR normalV = XMLoadFloat3(&primVertex1.normal);

			positiveAxesV = XMVectorMax(normalV, positiveAxesV);
			negativeAxesV = XMVectorMin(normalV, negativeAxesV);
		}
	}

	// The centre of the normals should be the cone normal.
	XMVECTOR normalCentre = XMVector3Normalize((positiveAxesV + negativeAxesV) * 0.5f);
	normalCentre          = XMVectorSetW(normalCentre, 0.f);

	// Check if the Cone is degenerate.
	XMVECTOR minimumDot = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		const Vertex& primVertex1 = vertices[vertexIndices[vertexOffset + unpackedIndices.firstIndex]];

		// All of the vertex normals of a triangle should point towards the same direction.

		XMVECTOR dot = XMVector3Dot(normalCentre, XMLoadFloat3(&primVertex1.normal));

		minimumDot = XMVectorMin(minimumDot, dot);
	}

	ClusterNormalCone normalCone{ 0u };

	// If the axis is more than 90 degrees, it is a degerate cone.
	if (XMVector4Less(minimumDot, XMVectorReplicate(0.1f)))
	{
		XMUBYTEN4 packedConeU{};
		XMStoreUByteN4(&packedConeU, XMVectorSet(0.f, 0.f, 0.f, 1.f));

		// Need to test this.
		normalCone.packedCone = packedConeU.v;
		normalCone.apexOffset = 0.f;

		return normalCone;
	}

	XMVECTOR spatialCentre = XMLoadFloat4(&meshletDetails.sphereB.sphere);

	float maxT = 0.f;

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		const Vertex& primVertex1 = vertices[vertexIndices[vertexOffset + unpackedIndices.firstIndex]];

		XMVECTOR centre = XMVectorSubtract(spatialCentre, XMLoadFloat3(&primVertex1.position));
		XMVECTOR normal = XMLoadFloat3(&primVertex1.normal);

		const float dotCentre = XMVectorGetX(XMVector3Dot(centre, normal));
		const float dotNormal = XMVectorGetX(XMVector3Dot(normalCentre, normal));

		assert(dotNormal > 0.f && "The dot normal should be bigger than the Minimum dot from above.");

		// The normal is a unit. Since we want to know how far the centre is, we are
		// dividing.
		const float t = dotCentre / dotNormal;

		maxT = std::max(t, maxT);
	}

	// MaxT is the apex offset.
	normalCone.apexOffset = maxT;

	// cos(Theta) for the normal cone is minimum dot; we need to add 90 degrees to the both sides
	// and invert the cone which gives us
	// -cos(theta + 90) = -(-sin(theta)) = sin(theta) = sqrt(1 - cos^2(a))
	XMVECTOR minimumDotSqr = XMVectorMultiply(minimumDot, minimumDot);
	XMVECTOR coneCutoff    = XMVectorSqrt(XMVectorSubtract(g_XMOne, minimumDotSqr));

	// Quantize the normal vector
	XMBYTEN4 snQuant{};
	XMStoreByteN4(&snQuant, normalCentre);

	std::uint8_t values[4]
	{
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.x) + 128),
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.y) + 128),
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.z) + 128),
		0u
	};

	// Bias
	XMVECTOR deQuant = XMLoadByteN4(&snQuant);
	XMVECTOR error   = XMVectorSum(XMVectorAbs(XMVectorSubtract(deQuant, normalCentre)));

	coneCutoff = XMVectorAdd(coneCutoff, error);

	// Quantize the normal cone spread to uint8 and bias upward.
	XMUBYTEN4 nQuant{};
	XMStoreUByteN4(&nQuant, coneCutoff);

	values[3] = nQuant.x;

	memcpy(&normalCone.packedCone, values, sizeof(std::uint32_t));

	return normalCone;
}

ClusterNormalCone GenerateNormalCone(
	aiVector3D* vertices, aiVector3D* normals, const std::vector<std::uint32_t>& vertexIndices,
	const std::vector<std::uint32_t>& primIndices, const MeshletDetails& meshletDetails
) noexcept {
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	const Meshlet& meshlet  = meshletDetails.meshlet;

	const auto vertexOffset = static_cast<size_t>(meshlet.vertexOffset);
	const auto primOffset   = static_cast<size_t>(meshlet.primitiveOffset);
	const auto primCount    = static_cast<size_t>(meshlet.primitiveCount);

	XMVECTOR positiveAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR negativeAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		XMFLOAT3 primNormal1 = GetXMFloat3(
			normals[vertexIndices[vertexOffset + unpackedIndices.firstIndex]]
		);

		// All of the vertex normals of a triangle should point towards the same direction.

		{
			// Calculate the AABB of the normals, so we can calculate the centre. Not using the other
			// functions, as the parameters would need to be different.
			XMVECTOR normalV = XMLoadFloat3(&primNormal1);

			positiveAxesV = XMVectorMax(normalV, positiveAxesV);
			negativeAxesV = XMVectorMin(normalV, negativeAxesV);
		}
	}

	// The centre of the normals should be the cone normal.
	XMVECTOR normalCentre = XMVector3Normalize((positiveAxesV + negativeAxesV) * 0.5f);
	normalCentre          = XMVectorSetW(normalCentre, 0.f);

	// Check if the Cone is degenerate.
	XMVECTOR minimumDot = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		XMFLOAT3 primNormal1 = GetXMFloat3(
			normals[vertexIndices[vertexOffset + unpackedIndices.firstIndex]]
		);

		// All of the vertex normals of a triangle should point towards the same direction.

		XMVECTOR dot = XMVector3Dot(normalCentre, XMLoadFloat3(&primNormal1));

		minimumDot = XMVectorMin(minimumDot, dot);
	}

	ClusterNormalCone normalCone{ 0u };

	// If the axis is more than 90 degrees, it is a degerate cone.
	if (XMVector4Less(minimumDot, XMVectorReplicate(0.1f)))
	{
		XMUBYTEN4 packedConeU{};
		XMStoreUByteN4(&packedConeU, XMVectorSet(0.f, 0.f, 0.f, 1.f));

		// Need to test this.
		normalCone.packedCone = packedConeU.v;
		normalCone.apexOffset = 0.f;

		return normalCone;
	}

	XMVECTOR spatialCentre = XMLoadFloat4(&meshletDetails.sphereB.sphere);

	float maxT = 0.f;

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		XMFLOAT3 primNormal1   = GetXMFloat3(
			normals[vertexIndices[vertexOffset + unpackedIndices.firstIndex]]
		);
		XMFLOAT3 primPosition1 = GetXMFloat3(
			vertices[vertexIndices[vertexOffset + unpackedIndices.firstIndex]]
		);

		XMVECTOR centre = XMVectorSubtract(spatialCentre, XMLoadFloat3(&primPosition1));
		XMVECTOR normal = XMLoadFloat3(&primNormal1);

		const float dotCentre = XMVectorGetX(XMVector3Dot(centre, normal));
		const float dotNormal = XMVectorGetX(XMVector3Dot(normalCentre, normal));

		assert(dotNormal > 0.f && "The dot normal should be bigger than the Minimum dot from above.");

		// The normal is a unit. Since we want to know how far the centre is, we are
		// dividing.
		const float t = dotCentre / dotNormal;

		maxT = std::max(t, maxT);
	}

	// MaxT is the apex offset.
	normalCone.apexOffset = maxT;

	// cos(Theta) for the normal cone is minimum dot; we need to add 90 degrees to the both sides
	// and invert the cone which gives us
	// -cos(theta + 90) = -(-sin(theta)) = sin(theta) = sqrt(1 - cos^2(a))
	XMVECTOR minimumDotSqr = XMVectorMultiply(minimumDot, minimumDot);
	XMVECTOR coneCutoff    = XMVectorSqrt(XMVectorSubtract(g_XMOne, minimumDotSqr));

	// Quantize the normal vector
	XMBYTEN4 snQuant{};
	XMStoreByteN4(&snQuant, normalCentre);

	std::uint8_t values[4]
	{
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.x) + 128),
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.y) + 128),
		static_cast<std::uint8_t>(static_cast<std::int16_t>(snQuant.z) + 128),
		0u
	};

	// Bias
	XMVECTOR deQuant = XMLoadByteN4(&snQuant);
	XMVECTOR error   = XMVectorSum(XMVectorAbs(XMVectorSubtract(deQuant, normalCentre)));

	coneCutoff = XMVectorAdd(coneCutoff, error);

	// Quantize the normal cone spread to uint8 and bias upward.
	XMUBYTEN4 nQuant{};
	XMStoreUByteN4(&nQuant, coneCutoff);

	values[3] = nQuant.x;

	memcpy(&normalCone.packedCone, values, sizeof(std::uint32_t));

	return normalCone;
}
