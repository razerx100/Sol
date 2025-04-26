#ifndef EXTERNAL_BINDING_INDICES_HPP_
#define EXTERNAL_BINDING_INDICES_HPP_
#include <cstdint>

namespace Sol
{
namespace ExternalBinding
{
	// Each type can have multiple techniques and among them, the one with the highest number of
	// binding will be here. The next type's bindings will start after that. That way we can have
	// constexpr binding indices, otherwise we will have to dynamically compile the shaders in the
	// runtime.
	static constexpr std::uint32_t s_lightCount = 0u;
	static constexpr std::uint32_t s_lightInfo  = 1u;
	static constexpr std::uint32_t s_material   = 2u;
	static constexpr std::uint32_t s_transparencyRenderTargetBindingData = 3u;
}
}
#endif
