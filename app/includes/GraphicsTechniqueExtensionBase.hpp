#ifndef GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#include <optional>
#include <GraphicsTechniqueExtension.hpp>

class GraphicsTechniqueExtensionBase : public GraphicsTechniqueExtension
{
public:
	GraphicsTechniqueExtensionBase() : m_externalBufferIndices{}, m_bufferBindingDetails{} {}

	void UpdateCPUData([[maybe_unused]] size_t frameIndex) noexcept override {}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetExternalBufferIndices() const noexcept override
	{
		return m_externalBufferIndices;
	}
	[[nodiscard]]
	const std::vector<ExternalBufferBindingDetails>& GetBindingDetails() const noexcept override
	{
		return m_bufferBindingDetails;
	}

protected:
	struct NewBufferInfo
	{
		size_t bufferSize;
		size_t instanceSize;
	};

	using NewBufferInfo_t = std::optional<NewBufferInfo>;

protected:
	[[nodiscard]]
	static NewBufferInfo_t GetNewBufferSize(
		const ExternalBuffer& buffer, size_t strideSize, size_t elementCount,
		size_t instanceCount = 1u, size_t extraAllocationCount = 0u
	) noexcept;

	template<typename T>
	[[nodiscard]]
	static NewBufferInfo_t GetNewBufferSize(
		const std::vector<T>& vec, const ExternalBuffer& buffer, size_t instanceCount = 1u,
		size_t extraAllocationCount = 0u
	) noexcept {
		return GetNewBufferSize(
			buffer, sizeof(T), std::size(vec), instanceCount, extraAllocationCount
		);
	}

	// Assuming the GPU isn't doing anything.
	template<class Renderer_t>
	void UpdateCPUBufferDescriptor(
		size_t bindingDetailsIndex, size_t frameIndex, size_t instanceSize,
		Renderer_t& renderer
	) {
		// Need to change some data, so can't do const ref.
		ExternalBufferBindingDetails bindingDetails = m_bufferBindingDetails[bindingDetailsIndex];

		bindingDetails.descriptorInfo.frameIndex    = static_cast<std::uint32_t>(frameIndex);
		bindingDetails.descriptorInfo.bufferSize    = instanceSize;
		bindingDetails.descriptorInfo.bufferOffset  = frameIndex * instanceSize;

		renderer.UpdateExternalBufferDescriptor(bindingDetails);
	}

	// Assuming the GPU isn't doing anything. And this descriptor will be bound on all the frames.
	template<class Renderer_t>
	void UpdateCPUBufferDescriptor(
		size_t bindingDetailsIndex, size_t bufferSize, Renderer_t& renderer
	) {
		// Need to change some data, so can't do const ref.
		ExternalBufferBindingDetails bindingDetails = m_bufferBindingDetails[bindingDetailsIndex];

		bindingDetails.descriptorInfo.frameIndex    = std::numeric_limits<std::uint32_t>::max();
		bindingDetails.descriptorInfo.bufferSize    = bufferSize;
		bindingDetails.descriptorInfo.bufferOffset  = 0;

		renderer.UpdateExternalBufferDescriptor(bindingDetails);
	}

protected:
	std::vector<std::uint32_t>                m_externalBufferIndices;
	std::vector<ExternalBufferBindingDetails> m_bufferBindingDetails;

public:
	GraphicsTechniqueExtensionBase(const GraphicsTechniqueExtensionBase&) = delete;
	GraphicsTechniqueExtensionBase& operator=(const GraphicsTechniqueExtensionBase&) = delete;

	GraphicsTechniqueExtensionBase(GraphicsTechniqueExtensionBase&& other) noexcept
		: m_externalBufferIndices{ std::move(other.m_externalBufferIndices) },
		m_bufferBindingDetails{ std::move(other.m_bufferBindingDetails) }
	{}

	GraphicsTechniqueExtensionBase& operator=(GraphicsTechniqueExtensionBase&& other) noexcept
	{
		m_externalBufferIndices = std::move(other.m_externalBufferIndices);
		m_bufferBindingDetails  = std::move(other.m_bufferBindingDetails);

		return *this;
	}
};
#endif
