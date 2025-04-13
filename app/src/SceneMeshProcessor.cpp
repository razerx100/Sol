#include <SceneMeshProcessor.hpp>
#include <MeshBoundImpl.hpp>
#include <MeshletMaker.hpp>

MeshBundleTemporaryData SceneMeshProcessor::GenerateTemporaryMeshData(bool meshShader)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiScene const* scene = m_scene->GetScene();

	if (meshShader)
		meshBundleTempData = GenerateMeshShaderData(scene);
	else
		meshBundleTempData = GenerateVertexShaderData(scene);

	return meshBundleTempData;
}

void SceneMeshProcessor::SetSceneProcessor(std::shared_ptr<SceneProcessor> scene)
{
	m_scene = std::move(scene);
}

MeshBundleTemporaryData SceneMeshProcessor::GenerateMeshShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsMS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshMS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

MeshBundleTemporaryData SceneMeshProcessor::GenerateVertexShaderData(aiScene const* scene)
{
	MeshBundleTemporaryData meshBundleTempData{};

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	meshBundleTempData.bundleDetails.meshTemporaryDetailsVS.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
		ProcessMeshVS(meshes[index], meshBundleTempData);

	return meshBundleTempData;
}

void SceneMeshProcessor::ProcessMeshVS(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshTemporaryDetailsVS meshDetailsVS
	{
		// Should be all triangles.
		.indexCount   = mesh->mNumFaces * 3u,
		.indexOffset  = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.aabb         = GetAABB(mesh->mAABB)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsVS.emplace_back(meshDetailsVS);

	const auto vertexOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices));

	ProcessMeshVertices(mesh, meshBundleTemporaryData);

	ProcessMeshFaces(mesh, vertexOffset, meshBundleTemporaryData);
}

void SceneMeshProcessor::ProcessMeshMS(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	MeshletMaker meshletMaker{};

	meshletMaker.GenerateMeshlets(mesh);

	std::vector<std::uint32_t> vertexIndices{};

	meshletMaker.LoadVertexIndices(vertexIndices);

	MeshExtraForMesh extraMeshData = meshletMaker.GenerateExtraMeshData();

	MeshTemporaryDetailsMS meshDetailsMS
	{
		.meshletCount    = static_cast<std::uint32_t>(std::size(extraMeshData.meshletDetails)),
		.meshletOffset   = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.meshletDetails)),
		.indexOffset     = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.indices)),
		.primitiveOffset = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.primIndices)),
		.vertexOffset    = static_cast<std::uint32_t>(std::size(meshBundleTemporaryData.vertices)),
		.aabb            = GetAABB(mesh->mAABB)
	};

	meshBundleTemporaryData.bundleDetails.meshTemporaryDetailsMS.emplace_back(meshDetailsMS);

	{
		// Per meshlet Bounding Sphere
		std::vector<MeshletDetails>& meshletDetails = extraMeshData.meshletDetails;

		const size_t meshletCount = std::size(meshletDetails);

		for (size_t index1 = 0u; index1 < meshletCount; ++index1)
		{
			MeshletDetails& meshletDetail = meshletDetails[index1];

			meshletDetail.sphereB = GenerateSphereBV(
				mesh->mVertices, vertexIndices, meshletDetail.meshlet
			);

			meshletDetail.coneNormal = GenerateNormalCone(
				mesh->mVertices, mesh->mNormals, vertexIndices, extraMeshData.primIndices,
				meshletDetail
			);
		}
	}

	MemcpyIntoVector(meshBundleTemporaryData.meshletDetails, extraMeshData.meshletDetails);
	MemcpyIntoVector(meshBundleTemporaryData.primIndices, extraMeshData.primIndices);
	MemcpyIntoVector(meshBundleTemporaryData.indices, vertexIndices);

	ProcessMeshVertices(mesh, meshBundleTemporaryData);
}

void SceneMeshProcessor::ProcessMeshVertices(
	aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	size_t newVertexCount = mesh->mNumVertices;
	aiVector3D* vertices  = mesh->mVertices;
	// Only using the first coordinates.
	aiVector3D* uvs       = mesh->mTextureCoords[0];
	aiVector3D* normals   = mesh->mNormals;

	std::vector<Vertex>& bundleVertices = meshBundleTemporaryData.vertices;

	bundleVertices.reserve(std::size(bundleVertices) + newVertexCount);

	if (mesh->HasTextureCoords(0))
	{
		for (size_t index = 0u; index < newVertexCount; ++index)
		{
			Vertex vertex
			{
				.position = GetXMFloat3(vertices[index]),
				.normal   = GetXMFloat3(normals[index]),
				.uv       = DirectX::XMFLOAT2{ uvs[index].x, uvs[index].y }
			};

			bundleVertices.emplace_back(vertex);
		}
	}
	else
	{
		for (size_t index = 0u; index < newVertexCount; ++index)
		{
			Vertex vertex
			{
				.position = GetXMFloat3(vertices[index]),
				.normal   = GetXMFloat3(normals[index]),
				.uv       = DirectX::XMFLOAT2{ 0.f, 0.f }
			};

			bundleVertices.emplace_back(vertex);
		}
	}
}

void SceneMeshProcessor::ProcessMeshFaces(
	aiMesh* mesh, std::uint32_t vertexOffset, MeshBundleTemporaryData& meshBundleTemporaryData
) noexcept {
	size_t faceCount           = mesh->mNumFaces;
	const size_t newIndexCount = faceCount * 3u;
	aiFace* faces              = mesh->mFaces;

	std::vector<std::uint32_t>& bundleIndices = meshBundleTemporaryData.indices;

	bundleIndices.reserve(std::size(bundleIndices) + newIndexCount);

	for (size_t index = 0u; index < faceCount; ++index)
	{
		const aiFace& face = faces[index];

		// Should be all triangles.
		std::uint32_t vIndex0 = vertexOffset + face.mIndices[0];
		std::uint32_t vIndex1 = vertexOffset + face.mIndices[1];
		std::uint32_t vIndex2 = vertexOffset + face.mIndices[2];

		bundleIndices.emplace_back(vIndex0);
		bundleIndices.emplace_back(vIndex1);
		bundleIndices.emplace_back(vIndex2);
	}
}
