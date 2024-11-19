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

std::shared_ptr<ModelBundleImpl> ModelBundleBase::GetBundleImpl() const noexcept
{
	auto bundleImpl               = std::make_shared<ModelBundleImpl>();
	bundleImpl->m_meshBundleIndex = m_meshBundleIndex;
	bundleImpl->m_models          = UpCastVector<Model>(m_models);

	return bundleImpl;
}

// Model Bundle Base
std::uint32_t ModelBundleBase::SetModelBundle(Renderer& renderer, const ShaderName& pixelShaderName) const
{
	return renderer.AddModelBundle(std::move(GetBundleImpl()), pixelShaderName);
}
