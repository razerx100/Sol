#include <ModelBase.hpp>
#include <concepts>
#include <ConversionUtilities.hpp>

ModelTransform::BrokenDownMatrix ModelTransform::BreakDownMatrix(
	const DirectX::XMMATRIX& matrix
) noexcept {
	using namespace DirectX;

	BrokenDownMatrix brokenDownMatrix{ .matrix = XMMatrixIdentity() };

	XMVECTOR scale{};
	XMVECTOR rotationQuat{};
	XMVECTOR translation{};

	XMMatrixDecompose(&scale, &rotationQuat, &translation, matrix);

	brokenDownMatrix.matrix *= XMMatrixScalingFromVector(scale);
	brokenDownMatrix.matrix *= XMMatrixRotationQuaternion(rotationQuat);

	XMStoreFloat3(&brokenDownMatrix.position, translation);

	brokenDownMatrix.scale = XMVectorGetX(scale);

	return brokenDownMatrix;
}

void ModelTransform::MultiplyAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
{
	using namespace DirectX;

	BrokenDownMatrix brokenDownMatrix = BreakDownMatrix(matrix);

	{
		XMVECTOR newOffset = XMLoadFloat3(&m_modelOffset) + XMLoadFloat3(&brokenDownMatrix.position);

		XMStoreFloat3(&m_modelOffset, newOffset);
	}

	m_modelMatrix *= brokenDownMatrix.matrix;
	m_modelScale   = brokenDownMatrix.scale;
}

void ModelTransform::SetAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
{
	using namespace DirectX;

	BrokenDownMatrix brokenDownMatrix = BreakDownMatrix(matrix);

	m_modelOffset = brokenDownMatrix.position;
	m_modelMatrix = brokenDownMatrix.matrix;
	m_modelScale  = brokenDownMatrix.scale;
}

void ModelTransform::RecalculateScale() noexcept
{
	DirectX::XMVECTOR scale{};
	DirectX::XMVECTOR rotationQuat{};
	DirectX::XMVECTOR translation{};

	DirectX::XMMatrixDecompose(&scale, &rotationQuat, &translation, m_modelMatrix);

	// We are scaling all of the components by the same amount.
	m_modelScale = DirectX::XMVectorGetX(scale);
}

template<typename To, std::derived_from<To> From>
static std::vector<std::shared_ptr<To>> UpCastVector(const std::vector<std::shared_ptr<From>>& from) noexcept
{
	std::vector<std::shared_ptr<To>> to{};

	const size_t elementCount = std::size(from);

	to.resize(elementCount);

	for (size_t index = 0u; index < elementCount; ++index)
		to[index] = from[index];

	return to;
}

// Pipeline Model Bundle Base
void PipelineModelBundleBase::AddModelIndex(std::uint32_t indexInBundle) noexcept
{
	m_modelIndicesInBundle.emplace_back(indexInBundle);
}

void PipelineModelBundleBase::RemoveModelIndex(std::uint32_t indexInBundle) noexcept
{
	std::erase(m_modelIndicesInBundle, indexInBundle);
}

// Model Bundle Base
ModelBundleBase& ModelBundleBase::AddModel(
	std::uint32_t pipelineIndex, float scale
) noexcept {
	return AddModel(pipelineIndex, std::make_shared<ModelBase>(scale));
}

ModelBundleBase& ModelBundleBase::AddModel(
	std::uint32_t pipelineIndex, std::shared_ptr<ModelBase> model
) noexcept {
	const std::uint32_t modelIndex = static_cast<std::uint32_t>(std::size(m_baseModels));

	m_modelNodeData.emplace_back(
		MeshNodeData
		{
			.modelIndex   = modelIndex,
			.meshIndex    = modelIndex,
			.childrenData = MeshChildrenData{ .count = 0u, .startingIndex = 0u }
		}
	);

	auto result = std::ranges::find(
		m_basePipelines, pipelineIndex,
		[](const std::shared_ptr<PipelineModelBundleBase>& pipeline)
		{
			return pipeline->GetPipelineIndex();
		}
	);

	PipelineModelBundleBase* targetPipeline = nullptr;

	if (result != std::end(m_basePipelines))
		targetPipeline = result->get();
	else
	{
		const size_t targetPipelineIndex = AddPipeline(pipelineIndex);

		targetPipeline = m_basePipelines[targetPipelineIndex].get();
	}

	targetPipeline->AddModelIndex(static_cast<std::uint32_t>(std::size(m_baseModels)));

	m_baseModels.emplace_back(model);
	m_models.emplace_back(std::move(model));

	return *this;
}

std::uint32_t ModelBundleBase::AddPipeline(std::uint32_t pipelineIndex) noexcept
{
	const auto pipelineIndexInBundle = static_cast<std::uint32_t>(std::size(m_basePipelines));

	auto basePipeline = std::make_shared<PipelineModelBundleBase>();

	basePipeline->SetPipelineIndex(pipelineIndex);

	m_basePipelines.emplace_back(basePipeline);
	m_pipelines.emplace_back(std::move(basePipeline));

	return pipelineIndexInBundle;
}

void ModelBundleBase::ChangeModelPipeline(
	std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
	std::uint32_t newPipelineIndex
) noexcept {
	size_t oldPipelineIndexInBundle = std::numeric_limits<size_t>::max();
	size_t newPipelineIndexInBundle = std::numeric_limits<size_t>::max();

	const size_t pipelineCount = std::size(m_basePipelines);

	for (size_t index = 0u; index < pipelineCount; ++index)
	{
		const size_t currentPipelineIndex = m_basePipelines[index]->GetPipelineIndex();

		if (currentPipelineIndex == oldPipelineIndex)
			oldPipelineIndexInBundle = index;

		if (currentPipelineIndex == newPipelineIndex)
			newPipelineIndexInBundle = index;

		const bool bothFound = oldPipelineIndexInBundle != std::numeric_limits<size_t>::max()
			&& newPipelineIndexInBundle != std::numeric_limits<size_t>::max();

		if (bothFound)
			break;
	}

	m_basePipelines[oldPipelineIndexInBundle]->RemoveModelIndex(modelIndexInBundle);
	m_basePipelines[newPipelineIndexInBundle]->AddModelIndex(modelIndexInBundle);
}

void ModelBundleBase::SetMeshBundle(
	std::uint32_t meshBundleIndex, float modelScale,
	const std::vector<MeshNodeData>& newNodeData,
	const std::vector<MeshPermanentDetails>& permanentDetails
) {
	SetModels(modelScale, newNodeData);

	ChangeMeshBundle(meshBundleIndex, newNodeData, permanentDetails, false);
}

void ModelBundleBase::SetModels(float modelScale, const std::vector<MeshNodeData>& nodeData)
{
	const size_t nodeCount = std::size(nodeData);

	m_modelNodeData.resize(nodeCount);

	for (size_t index = 0u; index < nodeCount; ++index)
	{
		const MeshNodeData& currentNodeData = nodeData[index];

		// Since any adjacent nodes without mesh will be skipped, the mesh index should
		// be the same ast the model index.
		if (currentNodeData.HasMesh())
		{
			std::shared_ptr<ModelBase>& model = m_baseModels.emplace_back(
				std::make_shared<ModelBase>(modelScale)
			);

			model->SetMeshIndex(currentNodeData.meshIndex);
		}
	}
}

void ModelBundleBase::ChangeMeshBundle(
	std::uint32_t meshBundleIndex,
	const std::vector<MeshNodeData>& newNodeData,
	const std::vector<MeshPermanentDetails>& permanentDetails,
	bool discardExistingTransformation
) {
	m_meshBundleIndex         = meshBundleIndex;
	const size_t newNodeCount = std::size(newNodeData);

	assert(!std::empty(m_baseModels) && "The models haven't been set yet.");

	assert(
		newNodeCount == std::size(m_modelNodeData)
		&& "The new mesh count isn't the same as before."
	);

	for (size_t index = 0u; index < newNodeCount; ++index)
	{
		const MeshNodeData& currentNodeData = newNodeData[index];
		m_modelNodeData[index]              = currentNodeData;

		if (currentNodeData.HasMesh())
		{
			const size_t modelIndex = currentNodeData.modelIndex;

			std::shared_ptr<ModelBase>& model       = m_baseModels[modelIndex];
			const MeshPermanentDetails& meshDetails = permanentDetails[modelIndex];

			// Base Colour
			const MeshTextureDetails& baseColourDetails = meshDetails.baseTextureDetails;

			ModelMaterial& material = model->GetMaterial();

			material.SetDiffuseIndex(baseColourDetails.baseTextureBindingIndex);
			material.SetDiffuseUVInfo(baseColourDetails.uvInfo);
			material.SetMaterialIndex(baseColourDetails.materialIndex);

			// Transform
			ModelTransform& transform = model->GetTransform();

			if (discardExistingTransformation)
				transform.ResetTransform();

			transform.MultiplyAndBreakDownModelMatrix(meshDetails.worldMatrix);
		}
	}
}

void ModelBundleBase::Rotate(
	size_t nodeIndex, const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	MeshNodeData nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		std::shared_ptr<ModelBase>& model = m_baseModels[nodeData.modelIndex];

		model->GetTransform().Rotate(rotationAxis, angleRadian);
	}

	const MeshChildrenData& childrenData = nodeData.childrenData;

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
	MeshNodeData nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		std::shared_ptr<ModelBase>& model = m_baseModels[nodeData.modelIndex];

		model->GetTransform().Scale(scale);
	}

	const MeshChildrenData& childrenData = nodeData.childrenData;

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
	MeshNodeData nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		std::shared_ptr<ModelBase>& model = m_baseModels[nodeData.modelIndex];

		model->GetTransform().MoveModel(offset);
	}

	const MeshChildrenData& childrenData = nodeData.childrenData;

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			MoveModel(childrenIndex, offset);
	}
}
