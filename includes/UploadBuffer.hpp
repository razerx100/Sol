#ifndef UPLOAD_BUFFER_HPP_
#define UPLOAD_BUFFER_HPP_
#include <memory>
#include <vector>

class UploadBuffer {
public:
	void AddData(std::shared_ptr<void> data) noexcept;

private:
	std::vector<std::shared_ptr<void>> m_uploadData;
};
#endif
