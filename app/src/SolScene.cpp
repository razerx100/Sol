#include <SolScene.hpp>
#include <ConversionUtilities.hpp>

namespace Sol
{
void SolScene::SetSceneNodes(const SceneProcessor& sceneProcessor)
{
	using namespace DirectX;

	aiScene const* scene   = sceneProcessor.GetScene();

	aiNode const* rootNode = scene->mRootNode;
	aiMesh** meshes        = scene->mMeshes;

	XMMATRIX accumulatedTransform = XMMatrixIdentity();
	std::uint32_t childrenOffset  = 1u;

	const size_t meshCount   = scene->mNumMeshes;

	std::uint32_t modelIndex = 0u;

	// There can be more nodes than the mesh count, but can't get that value here. So, in some
	// cases this might cause no extra allocations?
	m_sceneNodeData.reserve(meshCount);

	ProcessSceneNodeDetails(
		rootNode, m_sceneNodeData, accumulatedTransform, meshes, childrenOffset, modelIndex
	);

	// Calculate the transform for the root.
	// I pass row major matrices in the shaders, and assimp loads column major matrices.
	accumulatedTransform =
		accumulatedTransform * XMMatrixTranspose(GetXMMatrix(rootNode->mTransformation));

	TraverseMeshHierarchyDetails(
		rootNode, accumulatedTransform, m_sceneNodeData, meshes, childrenOffset, modelIndex
	);
}

void SolScene::SetMeshMaterialDetails(
	const SceneProcessor& sceneProcessor, const SceneMaterialProcessor& materialProcessor
) {
	aiScene const* scene   = sceneProcessor.GetScene();

	aiMesh** meshes        = scene->mMeshes;
	const size_t meshCount = scene->mNumMeshes;

	m_meshMaterialDetails.reserve(meshCount);

	for (size_t index = 0u; index < meshCount; ++index)
	{
		const size_t materialIndex = meshes[index]->mMaterialIndex;

		const SceneMaterialProcessor::MaterialDetails& tempMaterialDetails
			= materialProcessor.GetMaterialDetails(materialIndex);

		m_meshMaterialDetails.emplace_back(
			MeshMaterialDetails
			{
				.diffuseTextures  = tempMaterialDetails.diffuseDetails,
				.specularTextures = tempMaterialDetails.specularDetails,
				.materialIndex    = tempMaterialDetails.materialIndex,
				.pipelineIndex    = tempMaterialDetails.pipelineIndex
			}
		);
	}
}

void SolScene::ProcessSceneNodeDetails(
	aiNode const* node, std::vector<SceneNodeData>& sceneNodeData,
	DirectX::XMMATRIX accumulatedTransform, aiMesh** meshes, std::uint32_t& childrenOffset,
	std::uint32_t& modelIndex
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

			break;
		}
	}

	sceneNodeData.emplace_back(
		SceneNodeData
		{
			.modelNodeData = ModelNodeData
			{
				.modelIndex   = currentModelIndex,
				.meshIndex    = meshIndex,
				.childrenData = ModelNodeChildrenData
							{
								.count = childCount, .startingIndex = childrenOffset
							},

			},
			.worldMatrix = accumulatedTransform * DirectX::XMMatrixTranspose(
							GetXMMatrix(node->mTransformation)
						)
		}
	);

	childrenOffset += childCount;
}

void SolScene::TraverseMeshHierarchyDetails(
	aiNode const* node, DirectX::XMMATRIX accumulatedTransform,
	std::vector<SceneNodeData>& sceneNodeData,
	aiMesh** meshes, std::uint32_t& childrenOffset, std::uint32_t& modelIndex
) {
	using namespace DirectX;

	size_t childCount = node->mNumChildren;
	aiNode** children = node->mChildren;

	for (size_t index = 0u; index < childCount; ++index)
		ProcessSceneNodeDetails(
			children[index], sceneNodeData, accumulatedTransform, meshes, childrenOffset,
			modelIndex
		);

	for (size_t index = 0u; index < childCount; ++index)
	{
		aiNode const* child = children[index];

		XMMATRIX tempAccumulatedTransform = accumulatedTransform;

		tempAccumulatedTransform =
			tempAccumulatedTransform * XMMatrixTranspose(GetXMMatrix(child->mTransformation));

		TraverseMeshHierarchyDetails(
			child, tempAccumulatedTransform, sceneNodeData, meshes, childrenOffset, modelIndex
		);
	}
}
}
