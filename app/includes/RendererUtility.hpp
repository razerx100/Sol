#ifndef RENDERER_UTILITY_HPP_
#define RENDERER_UTILITY_HPP_
#include <ModelContainer.hpp>
#include <ModelBundle.hpp>

namespace Sol
{
namespace RendererUtility
{
	void RemoveModelBundle(
		const std::shared_ptr<ModelBundle>& modelBundle,
		std::shared_ptr<ModelContainer>& modelContainer
	) noexcept;

	template<class Renderer_t>
	void RemoveModelBundle(
		Renderer_t& renderer, std::shared_ptr<ModelContainer>& modelContainer,
		std::uint32_t modelBundleIndex
	) noexcept {
		std::shared_ptr<ModelBundle> modelBundle = renderer.RemoveModelBundle(modelBundleIndex);

		RemoveModelBundle(modelBundle, modelContainer);
	}
}
}
#endif
