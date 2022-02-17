#include <UploadBuffer.hpp>

void UploadBuffer::AddData(std::shared_ptr<void> data) noexcept {
	m_uploadData.emplace_back(data);
}

void UploadBuffer::Release() noexcept {
	for (auto& data : m_uploadData)
		data.reset();
}
