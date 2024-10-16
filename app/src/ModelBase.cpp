#include <ModelBase.hpp>
#include <concepts>

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

std::shared_ptr<ModelBundleImplVS> ModelBundleBaseVS::GetBundleImpl() const noexcept
{
	auto bundleImpl         = std::make_shared<ModelBundleImplVS>(ModelBundleImplVS{});
	bundleImpl->m_meshIndex = m_meshIndex;
	bundleImpl->m_models    = UpCastVector<ModelVS>(m_models);

	return bundleImpl;
}

std::shared_ptr<ModelBundleImplMS> ModelBundleBaseMS::GetBundleImpl() const noexcept
{
	auto bundleImpl         = std::make_shared<ModelBundleImplMS>(ModelBundleImplMS{});
	bundleImpl->m_meshIndex = m_meshIndex;
	bundleImpl->m_models    = UpCastVector<ModelMS>(m_models);

	return bundleImpl;
}

// Model Bundle
ModelBundle& ModelBundle::AddModel(float scale /* = 1.f */) noexcept
{
	if (!s_modelTypeVS)
		m_bundleMS->AddModel(std::make_shared<ModelBaseMS>(ModelBase{ scale }));
	else
		m_bundleVS->AddModel(std::make_shared<ModelBaseVS>(ModelBase{ scale }));

	return *this;
}

ModelBase& ModelBundle::GetModel(size_t index) noexcept
{
	if (!s_modelTypeVS)
		return m_bundleMS->GetModel(index)->GetBase();
	else
		return m_bundleVS->GetModel(index)->GetBase();
}

void ModelBundle::SetMeshModelDetails(size_t modelIndex, const MeshDetails& meshDetails) noexcept
{
	if (!s_modelTypeVS)
		m_bundleMS->GetModel(modelIndex)->SetMeshDetailsMS(
			MeshDetailsMS
			{
				.meshletCount  = meshDetails.elementCount,
				.meshletOffset = meshDetails.elementOffset
			}
		);
	else
		m_bundleVS->GetModel(modelIndex)->SetMeshDetailsVS(
			MeshDetailsVS
			{
				.indexCount  = meshDetails.elementCount,
				.indexOffset = meshDetails.elementOffset
			}
		);
}

void ModelBundle::SetMeshIndex(std::uint32_t index) noexcept
{
	if (!s_modelTypeVS)
		m_bundleMS->SetMeshIndex(index);
	else
		m_bundleVS->SetMeshIndex(index);
}

std::uint32_t ModelBundle::SetModelBundle(Renderer& renderer, const ShaderName& pixelShaderName)
{
	if (!s_modelTypeVS)
		return renderer.AddModelBundle(m_bundleMS->GetBundleImpl(), pixelShaderName);
	else
		return renderer.AddModelBundle(m_bundleVS->GetBundleImpl(), pixelShaderName);
}
