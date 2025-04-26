#include <MeshBundleBase.hpp>
#include <ranges>
#include <algorithm>
#include <cassert>
#include <MeshBoundImpl.hpp>
#include <BoundingVolumes.hpp>
#include <SolException.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <ConversionUtilities.hpp>
#include <MeshletMaker.hpp>

namespace Sol
{
// Mesh Bundle Temp Custom
MeshBundleTemporaryData MeshBundleTempCustom::GenerateTemporaryData(bool meshShader)
{
	MeshBundleTemporaryData meshBundleTempData{};

	if (meshShader)
		GenerateMeshShaderData(m_tempMeshes, meshBundleTempData);
	else
		GenerateVertexShaderData(m_tempMeshes, meshBundleTempData);

	m_tempMeshes = std::vector<Mesh>{};

	return meshBundleTempData;
}

void MeshBundleTempCustom::GenerateMeshShaderData(
	std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
) {
	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.reserve(std::size(meshes));

	for (Mesh& mesh : meshes)
		ProcessMeshMS(mesh, meshBundleTemporaryData);
}

void MeshBundleTempCustom::GenerateVertexShaderData(
	std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
) {
	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.reserve(std::size(meshes));

	for (Mesh& mesh : meshes)
		ProcessMeshVS(mesh, meshBundleTemporaryData);
}

void MeshBundleTempCustom::ProcessMeshVS(
	Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshTemporaryDetailsVS meshDetailsVS
	{
		.indexCount   = static_cast<std::uint32_t>(std::size(mesh.indices)),
		.indexOffset  = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.aabb         = GenerateAABB(mesh.vertices)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.emplace_back(meshDetailsVS);

	const auto vertexOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices));

	CopyAndOffsetIndices(meshBundleTemporaryData.indices, mesh.indices, vertexOffset);
	MemcpyIntoVector(meshBundleTemporaryData.vertices, mesh.vertices);
}

void MeshBundleTempCustom::ProcessMeshMS(
	Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshletMaker meshletMaker{};
	meshletMaker.GenerateMeshlets(mesh);

	meshletMaker.LoadVertexIndices(mesh.indices);

	MeshExtraForMesh extraMeshData = meshletMaker.GenerateExtraMeshData();

	MeshTemporaryDetailsMS meshDetailsMS
	{
		.meshletCount    = static_cast<std::uint32_t>(std::size(extraMeshData.meshletDetails)),
		.meshletOffset   = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.meshletDetails)),
		.indexOffset     = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.primitiveOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.primIndices)),
		.vertexOffset    = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices)),
		.aabb            = GenerateAABB(mesh.vertices)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.emplace_back(meshDetailsMS);

	{
		// Per meshlet Bounding Sphere
		std::vector<MeshletDetails>& meshletDetails = extraMeshData.meshletDetails;

		const size_t meshletCount = std::size(meshletDetails);

		for (size_t index = 0u; index < meshletCount; ++index)
		{
			MeshletDetails& meshletDetail = meshletDetails[index];

			meshletDetail.sphereB = GenerateSphereBV(
				mesh.vertices, mesh.indices, meshletDetail.meshlet
			);

			meshletDetail.coneNormal = GenerateNormalCone(
				mesh.vertices, mesh.indices, extraMeshData.primIndices, meshletDetail
			);
		}
	}

	MemcpyIntoVector(meshBundleTemporaryData.meshletDetails, extraMeshData.meshletDetails);
	MemcpyIntoVector(meshBundleTemporaryData.primIndices, extraMeshData.primIndices);
	MemcpyIntoVector(meshBundleTemporaryData.indices, mesh.indices);
	MemcpyIntoVector(meshBundleTemporaryData.vertices, mesh.vertices);
}

void MeshBundleTempCustom::AddMesh(Mesh&& mesh) noexcept
{
	m_tempMeshes.emplace_back(std::move(mesh));
}

// Mesh Bundle Temp Assimp
MeshBundleTemporaryData MeshBundleTempAssimp::GenerateTemporaryData(bool meshShader)
{
	return m_meshProcessor.GenerateTemporaryMeshData(meshShader);
}

void MeshBundleTempAssimp::SetSceneProcessor(std::shared_ptr<SceneProcessor> scene)
{
	m_meshProcessor.SetSceneProcessor(std::move(scene));
}

// Mesh Bundle Temporary Impl
MeshBundleTemporaryImpl::MeshBundleTemporaryImpl(bool customTemp)
	: m_tempData{},
	m_tempIntermediate{ customTemp ? std::make_unique<MeshBundleTempCustom>() : nullptr }
{}

void MeshBundleTemporaryImpl::GenerateTemporaryData(bool meshShader)
{
	m_tempData = m_tempIntermediate->GenerateTemporaryData(meshShader);
}

void MeshBundleTemporaryImpl::AddMesh(Mesh&& mesh)
{
	assert(
		dynamic_cast<MeshBundleTempCustom*>(m_tempIntermediate.get())
		&& "Temp type isn't custom."
	);

	m_tempIntermediate->AddMesh(std::move(mesh));
}

void MeshBundleTemporaryImpl::SetMeshBundle(std::shared_ptr<SceneProcessor> scene)
{
	if (!m_tempIntermediate)
	{
		auto tempData = std::make_unique<MeshBundleTempAssimp>();

		tempData->SetSceneProcessor(std::move(scene));

		m_tempIntermediate = std::move(tempData);
	}
}

// Mesh Bundle Impl
void MeshBundleImpl::AddMesh(Mesh&& mesh)
{
	m_temporaryData->AddMesh(std::move(mesh));
}

void MeshBundleImpl::SetMeshBundle(std::shared_ptr<SceneProcessor> scene)
{
	m_temporaryData->SetMeshBundle(std::move(scene));
}

void MeshBundleImpl::CalculateNormalsIndependentFaces(
	std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
) noexcept {
	for (size_t index = 0u; index < std::size(indices); index += 3)
	{
		Vertex& vertex1 = vertices[indices[index]];
		Vertex& vertex2 = vertices[indices[index + 1u]];
		Vertex& vertex3 = vertices[indices[index + 2u]];

		DirectX::XMFLOAT3 faceNormal = GetFaceNormal(
			vertex1.position, vertex2.position, vertex3.position
		);

		vertex1.normal = faceNormal;
		vertex2.normal = faceNormal;
		vertex3.normal = faceNormal;
	}
}

DirectX::XMFLOAT3 MeshBundleImpl::GetFaceNormal(
	const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
	const DirectX::XMFLOAT3& position3
) noexcept {
	DirectX::XMVECTOR vertex1 = DirectX::XMLoadFloat3(&position1);
	DirectX::XMVECTOR vertex2 = DirectX::XMLoadFloat3(&position2);
	DirectX::XMVECTOR vertex3 = DirectX::XMLoadFloat3(&position3);

	// Edges should be calculated in this subtraction order for ClockWise triangle vertices
	// drawing order
	DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(vertex2, vertex1);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(vertex3, vertex1);

	DirectX::XMVECTOR faceNormalV = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(edge1, edge2)
	);
	DirectX::XMFLOAT3 faceNormal{};
	DirectX::XMStoreFloat3(&faceNormal, faceNormalV);

	return faceNormal;
}

void MeshBundleImpl::SetUVToVertices(
	std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
) noexcept {
	for (size_t index = 0u; index < std::size(uvs); ++index)
		vertices[index].uv = uvs[index];
}
}
