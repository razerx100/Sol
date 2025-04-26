#include <GraphicsTechniqueExtensionBase.hpp>
#include <limits>

namespace Sol
{
GraphicsTechniqueExtensionBase::NewBufferInfo_t GraphicsTechniqueExtensionBase::GetNewBufferSize(
	const ExternalBuffer& buffer, size_t strideSize, size_t elementCount,
	size_t instanceCount, size_t extraAllocationCount
) noexcept {
	const size_t currentBufferSize   = buffer.BufferSize();
	const size_t minimumInstanceSize = elementCount * strideSize;
	const size_t requiredMinimumSize = minimumInstanceSize * instanceCount;

	NewBufferInfo_t newSize{};

	if (currentBufferSize < requiredMinimumSize)
	{
		const size_t extraAllocationInstanceSize = extraAllocationCount * strideSize;
		const size_t extraAllocationSize         = extraAllocationInstanceSize * instanceCount;

		newSize = NewBufferInfo
		{
			.bufferSize   = requiredMinimumSize + extraAllocationSize,
			.instanceSize = minimumInstanceSize + extraAllocationInstanceSize
		};
	}

	return newSize;
}
}
