#include <ModelBase.hpp>
#include <concepts>

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

// Model Bundle Base
std::shared_ptr<ModelBundleImpl> ModelBundleBase::GetBundleImpl() const noexcept
{
	auto bundleImpl               = std::make_shared<ModelBundleImpl>();
	bundleImpl->m_meshBundleIndex = m_meshBundleIndex;
	bundleImpl->m_models          = UpCastVector<Model>(m_models);

	return bundleImpl;
}

std::uint32_t ModelBundleBase::SetModelBundle(Renderer& renderer, const ShaderName& pixelShaderName) const
{
	return renderer.AddModelBundle(std::move(GetBundleImpl()), pixelShaderName);
}

void ModelBundleBase::SetMeshBundle(
	std::uint32_t meshBundleIndex, float modelScale, const MeshBundleImpl& meshBundle
) {
	SetModels(modelScale, meshBundle);

	ChangeMeshBundle(meshBundleIndex, meshBundle);
}

void ModelBundleBase::SetModels(float modelScale, const MeshBundleImpl& meshBundle)
{
	const std::vector<MeshNodeData>& nodeData = meshBundle.GetMeshNodeData();

	const size_t nodeCount = std::size(nodeData);

	m_modelNodeData.resize(nodeCount);

	for (size_t index = 0u; index < nodeCount; ++index)
	{
		const MeshNodeData& currentNodeData = nodeData[index];

		// Since any adjacent nodes without mesh will be skipped, the mesh index should
		// be the same ast the model index.
		if (currentNodeData.HasMesh())
		{
			std::shared_ptr<ModelBase>& model = m_models.emplace_back(
				std::make_shared<ModelBase>(modelScale)
			);

			model->SetMeshIndex(static_cast<std::uint32_t>(currentNodeData.meshIndex));
		}
	}
}

void ModelBundleBase::ChangeMeshBundle(std::uint32_t meshBundleIndex, const MeshBundleImpl& meshBundle)
{
	*m_meshBundleIndex = meshBundleIndex;

	const std::vector<MeshNodeData>& newNodeData              = meshBundle.GetMeshNodeData();
	const std::vector<MeshPermanentDetails>& permanentDetails = meshBundle.GetPermanentDetails();

	const size_t newNodeCount = std::size(newNodeData);

	assert(!std::empty(m_models) && "The models haven't been set yet.");

	assert(
		newNodeCount == std::size(m_modelNodeData)
		&& "The new mesh count isn't the same as before."
	);

	for (size_t index = 0u; index < newNodeCount; ++index)
	{
		const MeshNodeData& currentNodeData = newNodeData[index];

		m_modelNodeData[index] = currentNodeData;

		if (currentNodeData.HasMesh())
		{
			ModelTransform& transform = m_models[currentNodeData.meshIndex]->GetTransform();

			transform.SetModelMatrix(permanentDetails[index].worldMatrix);
		}
	}
}

void ModelBundleBase::Rotate(
	size_t nodeIndex, const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	MeshNodeData nodeData = m_modelNodeData[nodeIndex];

	if (nodeData.HasMesh())
	{
		std::shared_ptr<ModelBase>& model = m_models[nodeData.meshIndex];

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
		std::shared_ptr<ModelBase>& model = m_models[nodeData.meshIndex];

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
		std::shared_ptr<ModelBase>& model = m_models[nodeData.meshIndex];

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
