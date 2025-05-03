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
	GraphicsTechniqueExtension() : m_externalBufferIndices{}, m_bufferBindingDetails{} {}
	GraphicsTechniqueExtension(
		std::vector<std::uint32_t> bufferIndices,
		std::vector<ExternalBufferBindingDetails> bindingDetails
	) : m_externalBufferIndices{ std::move(bufferIndices) },
		m_bufferBindingDetails{ std::move(bindingDetails) }
	{}

	void AddExternalBufferIndex(std::uint32_t bufferIndex) noexcept
	{
		m_externalBufferIndices.emplace_back(bufferIndex);
	}

	void AddBindingDetails(const ExternalBufferBindingDetails& bindingDetails) noexcept
	{
		m_bufferBindingDetails.emplace_back(bindingDetails);
	}

	void SetExternalBufferIndices(std::vector<std::uint32_t> bufferIndices) noexcept
	{
		m_externalBufferIndices = std::move(bufferIndices);
	}

	void SetBindingDetails(std::vector<ExternalBufferBindingDetails> bindingDetails) noexcept
	{
		m_bufferBindingDetails = std::move(bindingDetails);
	}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetExternalBufferIndices() const noexcept
	{
		return m_externalBufferIndices;
	}
	[[nodiscard]]
	const std::vector<ExternalBufferBindingDetails>& GetBindingDetails() const noexcept
	{
		return m_bufferBindingDetails;
	}

private:
	std::vector<std::uint32_t>                m_externalBufferIndices;
	std::vector<ExternalBufferBindingDetails> m_bufferBindingDetails;

public:
	GraphicsTechniqueExtension(const GraphicsTechniqueExtension&) = delete;
	GraphicsTechniqueExtension& operator=(const GraphicsTechniqueExtension&) = delete;

	GraphicsTechniqueExtension(GraphicsTechniqueExtension&& other) noexcept
		: m_externalBufferIndices{ std::move(other.m_externalBufferIndices) },
		m_bufferBindingDetails{ std::move(other.m_bufferBindingDetails) }
	{}
	GraphicsTechniqueExtension& operator=(GraphicsTechniqueExtension&& other) noexcept
	{
		m_externalBufferIndices = std::move(other.m_externalBufferIndices);
		m_bufferBindingDetails  = std::move(other.m_bufferBindingDetails);

		return *this;
	}
};
#endif
