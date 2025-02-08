#ifndef GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#include <optional>
#include <GraphicsTechniqueExtension.hpp>
#include <Renderer.hpp>

class GraphicsTechniqueExtensionBase : public GraphicsTechniqueExtension
{
public:
	GraphicsTechniqueExtensionBase(Renderer* renderer)
		: m_externalBufferIndices{}, m_bufferBindingDetails{}, m_renderer{ renderer }
	{}

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
		return GetNewBufferSize(buffer, sizeof(T), std::size(vec), instanceCount, extraAllocationCount);
	}

	// Assuming the GPU isn't doing anything.
	void UpdateCPUBufferDescriptor(size_t bindingDetailsIndex, size_t frameIndex, size_t instanceSize);
	// Assuming the GPU isn't doing anything. And this descriptor will be bound on all the frames.
	void UpdateCPUBufferDescriptor(size_t bindingDetailsIndex, size_t bufferSize);

protected:
	std::vector<std::uint32_t>                m_externalBufferIndices;
	std::vector<ExternalBufferBindingDetails> m_bufferBindingDetails;
	Renderer*                                 m_renderer;

public:
	GraphicsTechniqueExtensionBase(const GraphicsTechniqueExtensionBase&) = delete;
	GraphicsTechniqueExtensionBase& operator=(const GraphicsTechniqueExtensionBase&) = delete;

	GraphicsTechniqueExtensionBase(GraphicsTechniqueExtensionBase&& other) noexcept
		: m_externalBufferIndices{ std::move(other.m_externalBufferIndices) },
		m_bufferBindingDetails{ std::move(other.m_bufferBindingDetails) },
		m_renderer{ other.m_renderer }
	{}

	GraphicsTechniqueExtensionBase& operator=(GraphicsTechniqueExtensionBase&& other) noexcept
	{
		m_externalBufferIndices = std::move(other.m_externalBufferIndices);
		m_bufferBindingDetails  = std::move(other.m_bufferBindingDetails);
		m_renderer              = other.m_renderer;

		return *this;
	}
};
#endif
