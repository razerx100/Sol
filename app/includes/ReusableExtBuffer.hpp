#ifndef REUSABLE_EXT_BUFFER_HPP_
#define REUSABLE_EXT_BUFFER_HPP_
#include <vector>
#include <memory>
#include <cstring>
#include <ExternalBuffer.hpp>
#include <ReusableVector.hpp>

namespace Sol
{
// This class should be used for single instance CPU buffers, where we would only update
// data when a new item is added.
template<class T, class ExternalBufferImpl_t>
class ReusableCPUExtBuffer
{
	using ExternalBuffer_t = ExternalBuffer<ExternalBufferImpl_t>;

public:
	ReusableCPUExtBuffer() : m_buffer{}, m_elements{} {}

	void SetCPUExtBuffer(std::shared_ptr<ExternalBufferImpl_t> buffer) noexcept
	{
		m_buffer.SetBufferImpl(std::move(buffer));
	}

	template<class U>
	[[nodiscard]]
	// Returns the index of the element in the ElementBuffer.
	size_t Add(U&& element)
	{
		const size_t oldCount     = std::size(m_elements);

		const size_t elementIndex = m_elements.Add(std::forward<U>(element), s_extraElementCount);

		const size_t newCount     = std::size(m_elements);

		if (newCount > oldCount)
			ExtendBuffer();
		else
			Update(elementIndex);

		return elementIndex;
	}

	void Update(size_t index, T&& element)
	{
		m_elements[index] = std::forward<T>(element);

		Update(index);
	}

	void Remove(size_t index) noexcept { m_elements.RemoveElement(index); }

	[[nodiscard]]
	auto&& GetExtBuffer(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_buffer);
	}

private:
	void Update(size_t index) noexcept
	{
		std::uint8_t* bufferStart = m_buffer.CPUHandle();
		constexpr size_t stride   = sizeof(T);
		size_t elementOffset      = index * stride;

		memcpy(bufferStart + elementOffset, &m_elements[index], stride);
	}

	void ExtendBuffer()
	{
		const size_t newBufferSize = std::size(m_elements) * sizeof(T);

		m_buffer.Create(newBufferSize);

		std::uint8_t* bufferStart = m_buffer.CPUHandle();

		memcpy(bufferStart, std::data(m_elements), newBufferSize);
	}

private:
	ExternalBuffer_t            m_buffer;
	Callisto::ReusableVector<T> m_elements;

	static constexpr size_t s_extraElementCount = 4u;

public:
	ReusableCPUExtBuffer(const ReusableCPUExtBuffer&) = delete;
	ReusableCPUExtBuffer& operator=(const ReusableCPUExtBuffer&) = delete;

	ReusableCPUExtBuffer(ReusableCPUExtBuffer&& other) noexcept
		: m_buffer{ std::move(other.m_buffer) }, m_elements{ std::move(other.m_elements) }
	{}
	ReusableCPUExtBuffer& operator=(ReusableCPUExtBuffer&& other) noexcept
	{
		m_buffer   = std::move(other.m_buffer);
		m_elements = std::move(other.m_elements);

		return *this;
	}
};
}
#endif
