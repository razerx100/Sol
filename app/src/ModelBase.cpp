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
