#ifndef GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <Shader.hpp>
#include <ExternalBuffer.hpp>

struct ExternalBufferDetails
{
	std::uint32_t      bufferId;
	ExternalBufferType type;
	size_t             size;
};

struct ExternalBufferBindingDetails
{
	// If the bindingIndex is the same, then the frameIndex must be different.
	std::uint32_t bindingIndex;
	std::uint32_t externalBufferIndex;
	std::uint32_t frameIndex;
	size_t        bufferOffset;
	size_t        bufferSize;
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

	[[nodiscard]]
	virtual const ShaderName& GetShaderName() const noexcept = 0;
};
#endif
