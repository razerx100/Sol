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

void SceneMeshProcessor::LoadMeshNodeDetails(
	const SceneMaterialProcessor& materialProcessor,
	std::vector<MeshPermanentDetails>& permanentDetails, std::vector<MeshNodeData>& meshNodeData
) {
	using namespace DirectX;

	aiScene const* scene          = m_scene->GetScene();

	aiNode const* rootNode        = scene->mRootNode;
	aiMesh** meshes               = scene->mMeshes;

	XMMATRIX accumulatedTransform = XMMatrixIdentity();
	std::uint32_t childrenOffset  = 1u;

	const size_t meshCount   = rootNode->mNumMeshes;

	std::uint32_t modelIndex = 0u;

	// There can be more nodes than the mesh count, but in most cases it won't be.
	permanentDetails.reserve(meshCount);
	meshNodeData.reserve(meshCount);

	ProcessMeshNodeDetails(
		rootNode, meshNodeData, materialProcessor, accumulatedTransform, permanentDetails, meshes,
		childrenOffset, modelIndex
	);

	if (rootNode->mNumMeshes)
		// Calculate the transform for the root.
		// I pass row major matrices in the shaders, and assimp loads column major matrices.
		accumulatedTransform =
			accumulatedTransform * XMMatrixTranspose(GetXMMatrix(rootNode->mTransformation));

	TraverseMeshHierarchyDetails(
		rootNode, materialProcessor,
		accumulatedTransform, permanentDetails, meshNodeData, meshes, childrenOffset, modelIndex
	);
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

void SceneMeshProcessor::ProcessMeshNodeDetails(
	aiNode const* node, std::vector<MeshNodeData>& meshNodeData,
	const SceneMaterialProcessor& materialProcessor, DirectX::XMMATRIX accumulatedTransform,
	std::vector<MeshPermanentDetails>& permanentDetails, aiMesh** meshes,
	std::uint32_t& childrenOffset, std::uint32_t& modelIndex
) {
	const std::uint32_t childCount = node->mNumChildren;

	auto currentModelIndex = std::numeric_limits<std::uint32_t>::max();
	auto meshIndex         = std::numeric_limits<std::uint32_t>::max();

	if (node->mNumMeshes)
	{
		const size_t meshCount = node->mNumMeshes;

		for (size_t index = 0u; index < meshCount; ++index)
		{
			size_t currentMeshIndex = node->mMeshes[index];
			aiMesh* mesh            = meshes[currentMeshIndex];

			const bool isTriangle
				= mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE;

			// Skipping non triangles.
			if (!isTriangle)
				continue;

			// Multiple meshes per node isn't supported yet.
			meshIndex = static_cast<std::uint32_t>(currentMeshIndex);

			currentModelIndex = modelIndex;
			++modelIndex;

			// Permanent Details
			{
				// Base Colour
				const size_t materialIndex = mesh->mMaterialIndex;

				const SceneMaterialProcessor::TextureDetails baseTextureDetailsM
					= materialProcessor.GetBaseTextureDetails(materialIndex);

				const SceneMaterialProcessor::MaterialDetails baseMaterialDetailsM
					= materialProcessor.GetMaterialDetails(materialIndex);

				MeshTextureDetails baseTextureDetails
				{
					.baseTextureIndex        = baseTextureDetailsM.textureIndex,
					.baseTextureBindingIndex = baseTextureDetailsM.textureBindIndex,
					.materialIndex           = baseMaterialDetailsM.materialIndex,
					.pipelineIndex           = baseMaterialDetailsM.pipelineIndex,
					.uvInfo                  = baseTextureDetailsM.uvInfo
				};

				// I pass row major matrices in the shaders, and assimp loads column major matrices.
				permanentDetails.emplace_back(
					MeshPermanentDetails
					{
						.worldMatrix = accumulatedTransform * DirectX::XMMatrixTranspose(
							GetXMMatrix(node->mTransformation)
						),
						.baseTextureDetails = baseTextureDetails
					}
				);
			}

			break;
		}
	}

	meshNodeData.emplace_back(
		MeshNodeData
		{
			.modelIndex   = currentModelIndex,
			.meshIndex    = meshIndex,
			.childrenData = MeshChildrenData{
								.count = childCount, .startingIndex = childrenOffset
							}
		}
	);

	childrenOffset += childCount;
}

void SceneMeshProcessor::TraverseMeshHierarchyDetails(
	aiNode const* node, const SceneMaterialProcessor& materialProcessor,
	DirectX::XMMATRIX accumulatedTransform, std::vector<MeshPermanentDetails>& permanentDetails,
	std::vector<MeshNodeData>& meshNodeData, aiMesh** meshes, std::uint32_t& childrenOffset,
	std::uint32_t& modelIndex
) {
	using namespace DirectX;

	size_t childCount = node->mNumChildren;
	aiNode** children = node->mChildren;

	for (size_t index = 0u; index < childCount; ++index)
		ProcessMeshNodeDetails(
			children[index], meshNodeData, materialProcessor, accumulatedTransform,
			permanentDetails, meshes, childrenOffset, modelIndex
		);

	for (size_t index = 0u; index < childCount; ++index)
	{
		aiNode const* child = children[index];

		XMMATRIX tempAccumulatedTransform = accumulatedTransform;

		if (child->mNumMeshes)
			tempAccumulatedTransform =
				tempAccumulatedTransform * XMMatrixTranspose(GetXMMatrix(child->mTransformation));

		TraverseMeshHierarchyDetails(
			child, materialProcessor, tempAccumulatedTransform, permanentDetails, meshNodeData,
			meshes, childrenOffset, modelIndex
		);
	}
}
