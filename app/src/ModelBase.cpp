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

void ModelBundleBase::SetMeshBundle(std::uint32_t index, const MeshBundleImpl& meshBundle) noexcept
{
	*m_meshBundleIndex = index;
	m_childrenData     = meshBundle.GetModelChildrenData();

	const std::vector<MeshPermanentDetails>& permanentDetails = meshBundle.GetPermanentDetails();

	for (size_t index1 = 0u; index1 < std::size(permanentDetails); ++index1)
	{
		ModelTransform& transform = m_models[index1]->GetTransform();

		transform.SetModelMatrix(permanentDetails[index1].worldMatrix);
	}
}

void ModelBundleBase::Rotate(
	size_t modelIndex, const DirectX::XMVECTOR& rotationAxis, float angleRadian
) noexcept {
	std::shared_ptr<ModelBase>& model = m_models[modelIndex];
	ModelChildren childrenData        = m_childrenData[modelIndex];

	model->GetTransform().Rotate(rotationAxis, angleRadian);

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			Rotate(childrenIndex, rotationAxis, angleRadian);
	}
}

void ModelBundleBase::Scale(size_t modelIndex, float scale) noexcept
{
	std::shared_ptr<ModelBase>& model = m_models[modelIndex];
	ModelChildren childrenData        = m_childrenData[modelIndex];

	model->GetTransform().Scale(scale);

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			Scale(childrenIndex, scale);
	}
}

void ModelBundleBase::MoveModel(size_t modelIndex, const DirectX::XMFLOAT3& offset) noexcept
{
	std::shared_ptr<ModelBase>& model = m_models[modelIndex];
	ModelChildren childrenData        = m_childrenData[modelIndex];

	model->GetTransform().MoveModel(offset);

	if (childrenData.count)
	{
		size_t childrenStart = childrenData.startingIndex;
		auto childrenEnd     = childrenStart + childrenData.count;

		for (size_t childrenIndex = childrenStart; childrenIndex < childrenEnd; ++childrenIndex)
			MoveModel(childrenIndex, offset);
	}
}
