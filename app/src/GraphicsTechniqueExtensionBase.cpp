#include <GraphicsTechniqueExtensionBase.hpp>

void GraphicsTechniqueExtensionBase::ExtendCPUVisibleBuffer(
	ExternalBuffer& cpuBuffer, ExternalBuffer& cpuTempBuffer, size_t newSize
) noexcept {
	cpuTempBuffer.Create(newSize);

	memcpy(cpuTempBuffer.CPUHandle(), cpuBuffer.CPUHandle(), cpuBuffer.BufferSize());

	cpuBuffer = std::move(cpuTempBuffer);
}


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

void GraphicsTechniqueExtensionBase::UpdateCPUBufferDescriptor(
	size_t bindingDetailsIndex, size_t frameIndex, size_t instanceSize
) {
	// Need to change some data, so can't do const ref.
	ExternalBufferBindingDetails bindingDetails = m_bufferBindingDetails[bindingDetailsIndex];

	bindingDetails.descriptorInfo.frameIndex    = static_cast<std::uint32_t>(frameIndex);
	bindingDetails.descriptorInfo.bufferSize    = instanceSize;
	bindingDetails.descriptorInfo.bufferOffset  = frameIndex * instanceSize;

	m_renderer->UpdateExternalBufferDescriptor(bindingDetails);
}
