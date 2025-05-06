#include <ModelBase.hpp>
#include <concepts>
#include <ConversionUtilities.hpp>

namespace Sol
{
// Model Bundle Base
ModelBundleBase& ModelBundleBase::AddModel(
	std::uint32_t pipelineIndex, float scale
) noexcept {
	return AddModel(pipelineIndex, std::make_shared<Model>(scale));
}

ModelBundleBase& ModelBundleBase::AddModel(
	std::uint32_t pipelineIndex, std::shared_ptr<Model> model
) noexcept {
	ModelContainer_t& models       = m_modelBundle->GetModels();
	PipelineContainer_t& pipelines = m_modelBundle->GetPipelineBundles();

	const std::uint32_t modelIndex = static_cast<std::uint32_t>(std::size(models));

	m_modelNodeData.emplace_back(
		ModelNodeData
		{
			.modelIndex   = modelIndex,
			.meshIndex    = modelIndex,
			.childrenData = ModelNodeChildrenData{ .count = 0u, .startingIndex = 0u }
		}
	);

	const size_t pipelineLocalIndex = GetLocalPipelineIndex(pipelineIndex);

	pipelines[pipelineLocalIndex].AddModelIndex(
		static_cast<std::uint32_t>(std::size(models))
	);

	models.emplace_back(std::move(model));

	return *this;
}

size_t ModelBundleBase::GetLocalPipelineIndex(std::uint32_t pipelineIndex)
{
	size_t pipelineLocalIndex  = std::numeric_limits<size_t>::max();

	std::optional<size_t> oPipelineLocalIndex = m_modelBundle->FindLocalPipelineIndex(
		pipelineIndex
	);

	if (!oPipelineLocalIndex)
		pipelineLocalIndex = AddPipeline(pipelineIndex);
	else
		pipelineLocalIndex = *oPipelineLocalIndex;

	return pipelineLocalIndex;
}

std::uint32_t ModelBundleBase::AddPipeline(std::uint32_t pipelineIndex) noexcept
{
	PipelineContainer_t& pipelines = m_modelBundle->GetPipelineBundles();

	const auto pipelineIndexInBundle = static_cast<std::uint32_t>(std::size(pipelines));

	auto pipeline = PipelineModelBundle{};

	pipeline.SetPipelineIndex(pipelineIndex);

	pipelines.emplace_back(std::move(pipeline));

	return pipelineIndexInBundle;
}

void ModelBundleBase::ChangeModelPipeline(
	std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
	std::uint32_t newPipelineIndex
) noexcept {
	size_t oldPipelineIndexInBundle = std::numeric_limits<size_t>::max();
	size_t newPipelineIndexInBundle = std::numeric_limits<size_t>::max();

	PipelineContainer_t& pipelines = m_modelBundle->GetPipelineBundles();

	const size_t pipelineCount = std::size(pipelines);

	for (size_t index = 0u; index < pipelineCount; ++index)
	{
		const size_t currentPipelineIndex = pipelines[index].GetPipelineIndex();

		if (currentPipelineIndex == oldPipelineIndex)
			oldPipelineIndexInBundle = index;

		if (currentPipelineIndex == newPipelineIndex)
			newPipelineIndexInBundle = index;

		const bool bothFound = oldPipelineIndexInBundle != std::numeric_limits<size_t>::max()
			&& newPipelineIndexInBundle != std::numeric_limits<size_t>::max();

		if (bothFound)
			break;
	}

	pipelines[oldPipelineIndexInBundle].RemoveModelIndex(modelIndexInBundle);
	pipelines[newPipelineIndexInBundle].AddModelIndex(modelIndexInBundle);
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
	ModelContainer_t& models = m_modelBundle->GetModels();

	const size_t nodeCount = std::size(sceneNodeData);

	for (size_t index = 0u; index < nodeCount; ++index)
	{
		const ModelNodeData& currentNodeData = sceneNodeData[index].modelNodeData;

		// Since any adjacent nodes without mesh will be skipped, the mesh index should
		// be the same ast the model index.
		if (currentNodeData.HasMesh())
		{
			auto model = std::make_shared<Model>(modelScale);

			model->SetMeshIndex(currentNodeData.meshIndex);

			models.emplace_back(std::move(model));
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
	ModelContainer_t& models       = m_modelBundle->GetModels();
	PipelineContainer_t& pipelines = m_modelBundle->GetPipelineBundles();

	m_modelBundle->SetMeshBundleIndex(meshBundleIndex);

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

			std::shared_ptr<Model>& model = models[modelIndex];
			const MeshMaterialDetails& oneMeshMaterialDetails = meshMaterialDetails[meshIndex];

			SetMaterial(model->GetMaterial(), oneMeshMaterialDetails);

			// Set Pipeline
			const size_t pipelineLocalIndex = GetLocalPipelineIndex(
				oneMeshMaterialDetails.pipelineIndex
			);

			pipelines[pipelineLocalIndex].AddModelIndex(
				static_cast<std::uint32_t>(modelIndex)
			);

			// Transform
			ModelTransform& transform = model->GetTransform();

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
		ModelContainer_t& models = m_modelBundle->GetModels();

		std::shared_ptr<Model>& model = models[nodeData.modelIndex];

		model->GetTransform().Rotate(rotationAxis, angleRadian);
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
		ModelContainer_t& models = m_modelBundle->GetModels();

		std::shared_ptr<Model>& model = models[nodeData.modelIndex];

		model->GetTransform().Scale(scale);
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
		ModelContainer_t& models = m_modelBundle->GetModels();

		std::shared_ptr<Model>& model = models[nodeData.modelIndex];

		model->GetTransform().MoveModel(offset);
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
