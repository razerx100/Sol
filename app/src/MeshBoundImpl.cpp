#include <ranges>
#include <algorithm>
#include <cmath>
#include <MeshBoundImpl.hpp>
#include <MeshletMaker.hpp>
#include <DirectXPackedVector.h>

// AABB Generator
void AABBGenerator::ProcessVertex(const DirectX::XMFLOAT3& position) noexcept
{
	using namespace DirectX;

	XMVECTOR vertexPosition = XMLoadFloat3(&position);

	m_positiveAxes = XMVectorMax(vertexPosition, m_positiveAxes);
	m_negativeAxes = XMVectorMin(vertexPosition, m_negativeAxes);
}

AxisAlignedBoundingBox AABBGenerator::GenerateAABB() const noexcept
{
	using namespace DirectX;

	AxisAlignedBoundingBox aabb{};

	XMFLOAT4& pAxes = aabb.maxAxes;
	XMFLOAT4& nAxes = aabb.minAxes;

	XMStoreFloat4(&pAxes, m_positiveAxes);
	XMStoreFloat4(&nAxes, m_negativeAxes);

	pAxes.w = 1.f;
	nAxes.w = 1.f;

	return aabb;
}

// Sphere BV Generator
void SphereBVGenerator::SetCentre(const AxisAlignedBoundingBox& aabb) noexcept
{
	using namespace DirectX;

	m_centre = (XMLoadFloat4(&aabb.maxAxes) + XMLoadFloat4(&aabb.minAxes)) * 0.5;
}

void SphereBVGenerator::ProcessVertex(const DirectX::XMFLOAT3& position) noexcept
{
	using namespace DirectX;

	XMVECTOR vertexV  = XMLoadFloat3(&position);
	XMVECTOR distance = XMVector3Length(m_centre - vertexV);

	m_radius = std::max(m_radius, XMVectorGetX(distance));
}

SphereBoundingVolume SphereBVGenerator::GenerateBV() const noexcept
{
	using namespace DirectX;

	// The first three components should be the centre. In the object space, it should be 0, 0.
	SphereBoundingVolume sphereVolume{ { 0.f, 0.f, 0.f, 0.f } };

	XMStoreFloat4(&sphereVolume.sphere, m_centre);

	sphereVolume.sphere.w = m_radius;

	return sphereVolume;
}

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

	AABBGenerator aabbGen{};

	for (const Vertex& vertex : vertices)
		aabbGen.ProcessVertex(vertex.position);

	return aabbGen.GenerateAABB();
}

AxisAlignedBoundingBox GenerateAABB(const std::vector<DirectX::XMFLOAT3>& positions) noexcept
{
	using namespace DirectX;

	AABBGenerator aabbGen{};

	for (const DirectX::XMFLOAT3& position : positions)
		aabbGen.ProcessVertex(position);

	return aabbGen.GenerateAABB();
}

SphereBoundingVolume GenerateSphereBV(const std::vector<Vertex>& vertices) noexcept
{
	using namespace DirectX;

	SphereBVGenerator sphereBVGen{};

	sphereBVGen.SetCentre(GenerateAABB(vertices));

	for (const Vertex& vertex : vertices)
		sphereBVGen.ProcessVertex(vertex.position);

	return sphereBVGen.GenerateBV();
}

SphereBoundingVolume GenerateSphereBV(const std::vector<DirectX::XMFLOAT3>& positions) noexcept
{
	using namespace DirectX;

	SphereBVGenerator sphereBVGen{};

	sphereBVGen.SetCentre(GenerateAABB(positions));

	for (const DirectX::XMFLOAT3& position : positions)
		sphereBVGen.ProcessVertex(position);

	return sphereBVGen.GenerateBV();
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

	AABBGenerator aabbGen{};

	const auto vertexEnd = static_cast<size_t>(meshlet.indexOffset + meshlet.indexCount);

	for (size_t index = meshlet.indexOffset; index < vertexEnd; ++index)
		aabbGen.ProcessVertex(GetPosition(vertices[vertexIndices[index]]));

	return aabbGen.GenerateAABB();
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

	SphereBVGenerator sphereBVGen{};

	sphereBVGen.SetCentre(_generateAABB(vertices, vertexIndices, meshlet));

	const auto vertexEnd = static_cast<size_t>(meshlet.indexOffset + meshlet.indexCount);

	for (size_t index = meshlet.indexOffset; index < vertexEnd; ++index)
		sphereBVGen.ProcessVertex(GetPosition(vertices[vertexIndices[index]]));

	return sphereBVGen.GenerateBV();
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

	const auto indexOffset = static_cast<size_t>(meshlet.indexOffset);
	const auto primOffset  = static_cast<size_t>(meshlet.primitiveOffset);
	const auto primCount   = static_cast<size_t>(meshlet.primitiveCount);

	AABBGenerator normalAABB{};

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		const Vertex& primVertex1
			= vertices[vertexIndices[indexOffset + unpackedIndices.firstIndex]];

		// All of the vertex normals of a triangle should point towards the same direction.

		// Calculate the AABB of the normals, so we can calculate the centre. Not using the other
		// functions, as the parameters would need to be different.
		normalAABB.ProcessVertex(primVertex1.normal);
	}

	// The centre of the normals should be the cone normal.
	XMVECTOR normalCentre = XMVector3Normalize(
		(normalAABB.m_positiveAxes + normalAABB.m_negativeAxes) * 0.5f
	);
	normalCentre = XMVectorSetW(normalCentre, 0.f);

	// Check if the Cone is degenerate.
	XMVECTOR minimumDot = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		const Vertex& primVertex1
			= vertices[vertexIndices[indexOffset + unpackedIndices.firstIndex]];

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

		const Vertex& primVertex1
			= vertices[vertexIndices[indexOffset + unpackedIndices.firstIndex]];

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

	const auto indexOffset = static_cast<size_t>(meshlet.indexOffset);
	const auto primOffset  = static_cast<size_t>(meshlet.primitiveOffset);
	const auto primCount   = static_cast<size_t>(meshlet.primitiveCount);

	XMVECTOR positiveAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	XMVECTOR negativeAxesV{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	for (size_t index = 0u; index < primCount; ++index)
	{
		std::uint32_t packedPrimIndices = primIndices[primOffset + index];

		MeshletMaker::PrimitiveIndicesUnpacked unpackedIndices =
			MeshletMaker::UnpackPrim(packedPrimIndices);

		XMFLOAT3 primNormal1 = GetXMFloat3(
			normals[vertexIndices[indexOffset + unpackedIndices.firstIndex]]
		);

		// All of the vertex normals of a triangle should point towards the same direction.

		{
			// Calculate the AABB of the normals, so we can calculate the centre. Not using the
			// other functions, as the parameters would need to be different.
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
			normals[vertexIndices[indexOffset + unpackedIndices.firstIndex]]
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
			normals[vertexIndices[indexOffset + unpackedIndices.firstIndex]]
		);
		XMFLOAT3 primPosition1 = GetXMFloat3(
			vertices[vertexIndices[indexOffset + unpackedIndices.firstIndex]]
		);

		XMVECTOR centre = XMVectorSubtract(spatialCentre, XMLoadFloat3(&primPosition1));
		XMVECTOR normal = XMLoadFloat3(&primNormal1);

		const float dotCentre = XMVectorGetX(XMVector3Dot(centre, normal));
		const float dotNormal = XMVectorGetX(XMVector3Dot(normalCentre, normal));

		assert(
			dotNormal > 0.f && "The dot normal should be bigger than the Minimum dot from above."
		);

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
