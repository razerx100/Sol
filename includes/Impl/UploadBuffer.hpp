#ifndef __UPLOAD_BUFFER_HPP__
#define __UPLOAD_BUFFER_HPP__
#include <IUploadBuffer.hpp>
#include <vector>

class UploadBuffer : public IUploadBuffer {
public:
	void AddData(std::shared_ptr<void> data) noexcept override;

	void Release() noexcept override;

private:
	std::vector<std::shared_ptr<void>> m_uploadData;
};
#endif
