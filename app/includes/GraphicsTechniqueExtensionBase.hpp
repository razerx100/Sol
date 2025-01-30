#ifndef GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#define GRAPHICS_TECHNIQUE_EXTENSION_BASE_HPP_
#include <optional>
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
	// This will copy the old buffer into the new one and set it to cpuBuffer and empty the temp buffer.
	static void ExtendCPUVisibleBuffer(
		ExternalBuffer& cpuBuffer, ExternalBuffer& cpuTempBuffer, size_t newSize
	) noexcept;

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
