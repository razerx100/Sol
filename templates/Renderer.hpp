#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>
#include <memory>
#include <array>
#include <IThreadPool.hpp>

#include <IModel.hpp>
#include <ISharedDataContainer.hpp>

class Renderer {
public:
	using Resolution = std::pair<std::uint64_t, std::uint64_t>;

	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;

	[[nodiscard]]
	virtual Resolution GetDisplayCoordinates(std::uint32_t displayIndex = 0u) const = 0;

	[[nodiscard]] // Returns index of the resource in Resource Heap
	virtual size_t RegisterResource(
		std::unique_ptr<std::uint8_t> textureData,
		size_t width, size_t height, bool components16bits = false
	) = 0;

	virtual void SetThreadPool(std::shared_ptr<IThreadPool> threadPoolArg) noexcept = 0;
	virtual void SetBackgroundColour(const std::array<float, 4>& colour) noexcept = 0;
	virtual void SubmitModel(std::shared_ptr<IModel> model) = 0;
	virtual void Render() = 0;
	virtual void WaitForAsyncTasks() = 0;

	virtual void SetShaderPath(const char* path) noexcept = 0;
	virtual void InitResourceBasedObjects() = 0;
	virtual void ProcessData() = 0;

	virtual void SetSharedDataContainer(
		std::shared_ptr<ISharedDataContainer> sharedData
	) noexcept = 0;
};
#endif
