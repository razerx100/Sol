#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>
#include <IModel.hpp>
#include <CRSStructures.hpp>
#include <memory>

class Renderer {
public:
	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(
		std::uint64_t& monitorWidth, std::uint64_t& monitorHeight
	) = 0;

	[[nodiscard]] // Returns index of the resource in Resource Heap
	virtual size_t RegisterResource(
		const void* data,
		size_t width, size_t height, size_t pixelSizeInBytes
	) = 0;

	virtual void SetThreadPool(std::shared_ptr<class IThreadPool> threadPoolArg) noexcept = 0;
	virtual void SetBackgroundColour(const Ceres::Float32_4& colour) noexcept = 0;
	virtual void SubmitModel(const class IModel* const modelRef) = 0;
	virtual void Render() = 0;
	virtual void WaitForAsyncTasks() = 0;

	virtual void SetShaderPath(const char* path) noexcept = 0;
	virtual void InitResourceBasedObjects() = 0;
	virtual void ProcessData() = 0;
};

#endif
