#ifndef __I_UPLOAD_BUFFER_HPP__
#define __I_UPLOAD_BUFFER_HPP__
#include <memory>

class IUploadBuffer {
public:
	virtual ~IUploadBuffer() = default;

	virtual void AddData(std::shared_ptr<void> data) noexcept = 0;

	virtual void Release() noexcept = 0;
};

IUploadBuffer* CreateUploadBufferInstance();
#endif
