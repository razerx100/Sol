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
}
