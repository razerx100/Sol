#include <ModelBase.hpp>
#include <concepts>
#include <ConversionUtilities.hpp>

namespace Sol
{
// Model Bundle Base
std::uint32_t ModelBundleBase::AddModel(std::uint32_t pipelineIndex, float scale) noexcept
{
	return AddModel(pipelineIndex, Model{ scale });
}

std::uint32_t ModelBundleBase::AddModel(std::uint32_t pipelineIndex, Model&& model) noexcept
{
	const std::uint32_t modelIndex = m_modelBundle->AddModel(std::move(model), pipelineIndex);

	m_modelNodeData.emplace_back(
		ModelNodeData
		{
			.modelIndex   = modelIndex,
			.meshIndex    = modelIndex,
			.childrenData = ModelNodeChildrenData{ .count = 0u, .startingIndex = 0u }
		}
	);

	return modelIndex;
}

void ModelBundleBase::ChangeModelPipeline(
	std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
	std::uint32_t newPipelineIndex
) noexcept {
	m_modelBundle->ChangeModelPipeline(modelIndexInBundle, oldPipelineIndex, newPipelineIndex);
}

void ModelBundleBase::SetMeshBundle(
	std::uint32_t meshBundleIndex, float modelScale,
	const std::vector<SceneNodeData>& sceneNodeData,
	const std::vector<MeshMaterialDetails>& meshMaterialDetails
) {
	SetModels(modelScale, sceneNodeData);

	ChangeMeshBundle(meshBundleIndex, sceneNodeData, meshMaterialDetails, false);
}

void ModelBundleBase::SetModels(
	float modelScale, const std::vector<SceneNodeData>& sceneNodeData
) {
	const size_t nodeCount = std::size(sceneNodeData);

	ModelBundle& modelBundle = *m_modelBundle;

	for (size_t index = 0u; index < nodeCount; ++index)
	{
		const ModelNodeData& currentNodeData = sceneNodeData[index].modelNodeData;

		// Since any adjacent nodes without mesh will be skipped, the mesh index should
		// be the same ast the model index.
		if (currentNodeData.HasMesh())
		{
			Model model{ modelScale };

			model.SetMeshIndex(currentNodeData.meshIndex);

			modelBundle.AddModel(std::move(model));
		}
	}
}

void ModelBundleBase::SetMaterial(
	ModelMaterial& material, const MeshMaterialDetails& materialDetails
) noexcept {
	const MeshTextureDetails& defaultTexture = GetDefaultTextureDetails();

	// For now we will only use the first texture and in the absence of that, the default
	// texture.
	MeshTextureDetails diffuseDetails  = defaultTexture;
	MeshTextureDetails specularDetails = defaultTexture;

	if (!std::empty(materialDetails.diffuseTextures))
		diffuseDetails = materialDetails.diffuseTextures.front();

	if (!std::empty(materialDetails.specularTextures))
		specularDetails = materialDetails.specularTextures.front();
	else if (!std::empty(materialDetails.diffuseTextures))
		specularDetails = materialDetails.diffuseTextures.front();

	material.SetDiffuseIndex(diffuseDetails.textureBindIndex);
	material.SetDiffuseUVInfo(diffuseDetails.uvInfo);

	material.SetSpecularIndex(specularDetails.textureBindIndex);
	material.SetSpecularUVInfo(specularDetails.uvInfo);

	material.SetMaterialIndex(materialDetails.materialIndex);
}

void ModelBundleBase::ChangeMeshBundle(
	std::uint32_t meshBundleIndex,
	const std::vector<SceneNodeData>& sceneNodeData,
	const std::vector<MeshMaterialDetails>& meshMaterialDetails,
	bool discardExistingTransformation
) {
	ModelBundle& modelBundle = *m_modelBundle;

	modelBundle.SetMeshBundleIndex(meshBundleIndex);

	const size_t newNodeCount = std::size(sceneNodeData);

	if (std::empty(m_modelNodeData))
		m_modelNodeData.resize(newNodeCount);

	for (size_t index = 0u; index < newNodeCount; ++index)
	{
		const SceneNodeData& currentNodeData  = sceneNodeData[index];
		const ModelNodeData& currentModelNode = currentNodeData.modelNodeData;

		m_modelNodeData[index] = currentModelNode;

		if (currentModelNode.HasMesh())
		{
			const size_t modelIndex = currentModelNode.modelIndex;
			const size_t meshIndex  = currentModelNode.meshIndex;

			Model& model = modelBundle.GetModel(modelIndex);

			const MeshMaterialDetails& oneMeshMaterialDetails = meshMaterialDetails[meshIndex];

			SetMaterial(model.GetMaterial(), oneMeshMaterialDetails);

			// Set Pipeline
			modelBundle.SetModelPipeline(modelIndex, oneMeshMaterialDetails.pipelineIndex);

			// Transform
			ModelTransform& transform = model.GetTransform();

			if (discardExistingTransformation)
				transform.ResetTransform();

			transform.MultiplyAndBreakDownModelMatrix(currentNodeData.worldMatrix);
		}
	}
}

void ModelBundleBase::Rotate(
	size_t nodeIndex, const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	const ModelNodeData& nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		Model& model = m_modelBundle->GetModel(nodeData.modelIndex);

		model.GetTransform().Rotate(rotationAxis, angleRadian);
	}

	const ModelNodeChildrenData& childrenData = nodeData.childrenData;

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			Rotate(childrenIndex, rotationAxis, angleRadian);
	}
}

void ModelBundleBase::Scale(size_t nodeIndex, float scale) noexcept
{
	const ModelNodeData& nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		Model& model = m_modelBundle->GetModel(nodeData.modelIndex);

		model.GetTransform().Scale(scale);
	}

	const ModelNodeChildrenData& childrenData = nodeData.childrenData;

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			Scale(childrenIndex, scale);
	}
}

void ModelBundleBase::MoveModel(size_t nodeIndex, const DirectX::XMFLOAT3& offset) noexcept
{
	const ModelNodeData& nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		Model& model = m_modelBundle->GetModel(nodeData.modelIndex);

		model.GetTransform().MoveModel(offset);
	}

	const ModelNodeChildrenData& childrenData = nodeData.childrenData;

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			MoveModel(childrenIndex, offset);
	}
}
}
