#include <RendererUtility.hpp>

namespace Sol
{
namespace RendererUtility
{
	void RemoveModelBundle(
		const std::shared_ptr<ModelBundle>& modelBundle,
		std::shared_ptr<ModelContainer>& modelContainer
	) noexcept {
		const std::vector<std::uint32_t>& modelBundleIndicesInContainer
			= modelBundle->GetIndicesInContainer();

		modelContainer->RemoveModels(modelBundleIndicesInContainer);
	}
}
}
