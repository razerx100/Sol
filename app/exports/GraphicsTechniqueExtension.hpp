#ifndef GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <ExternalBuffer.hpp>

struct ExternalBufferBindingDetails
{
	// If the bindingIndex is the same, then the frameIndex must be different.
	struct
	{
		std::uint32_t      bindingIndex;
		ExternalBufferType type;
	} layoutInfo; // Will be valid from the start.
	struct
	{
		std::uint32_t externalBufferIndex; // Will be valid after the buffer has been created.
		std::uint32_t frameIndex;          // Will be changed before a call to changing a descriptor.
		size_t        bufferOffset;        // Will be ever changing.
		size_t        bufferSize;          // Will be ever changing.
	} descriptorInfo;
};

class GraphicsTechniqueExtension
{
public:
	virtual ~GraphicsTechniqueExtension() = default;

	virtual void UpdateCPUData(size_t frameIndex) noexcept = 0;

	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetExternalBufferIndices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<ExternalBufferBindingDetails>& GetBindingDetails() const noexcept = 0;
};
#endif
