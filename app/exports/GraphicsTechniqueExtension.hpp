#ifndef GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <ExternalBuffer.hpp>

struct ExternalBufferDetails
{
	std::uint32_t      bufferId;
	ExternalBufferType type;
};

struct ExternalBufferBindingDetails
{
	// If the bindingIndex is the same, then the frameIndex must be different.
	std::uint32_t      bindingIndex;        // Will be valid from the start.
	std::uint32_t      externalBufferIndex; // Will be valid after the buffer has been created.
	std::uint32_t      frameIndex;          // Will be valid from the start.
	ExternalBufferType type;                // Will be valid from the start.
	size_t             bufferOffset;        // Will be ever changing.
	size_t             bufferSize;          // Will be ever changing.
};

class GraphicsTechniqueExtension
{
public:
	virtual ~GraphicsTechniqueExtension() = default;

	virtual void UpdateCPUData(size_t frameIndex) const noexcept = 0;

	virtual void SetBuffer(
		std::shared_ptr<ExternalBuffer> buffer, std::uint32_t bufferId,
		std::uint32_t externalBufferIndex
	) noexcept = 0;

	[[nodiscard]]
	// Will be used to create the necessary buffers.
	virtual const std::vector<ExternalBufferDetails>& GetBufferDetails() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<ExternalBufferBindingDetails>& GetBindingDetails() const noexcept = 0;
};
#endif
