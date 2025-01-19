#ifndef SOL_GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#define SOL_GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#include <GraphicsTechniqueExtension.hpp>

constexpr size_t operator"" _B(unsigned long long number) noexcept
{
	return static_cast<size_t>(number);
}

constexpr size_t operator"" _KB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u);
}

constexpr size_t operator"" _MB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u * 1024u);
}

constexpr size_t operator"" _GB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u * 1024u * 1024u);
}

class SolGraphicsTechniqueExtension : public GraphicsTechniqueExtension
{
public:
	virtual ~SolGraphicsTechniqueExtension() = default;

	virtual void UpdateCPUData([[maybe_unused]] size_t frameIndex) const noexcept override {};
};
#endif
