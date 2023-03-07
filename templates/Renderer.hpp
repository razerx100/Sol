#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>
#include <memory>
#include <array>
#include <string>
#include <IThreadPool.hpp>

#include <IModel.hpp>
#include <ISharedDataContainer.hpp>

enum class RenderEngineType {
	IndirectDraw,
	IndividualDraw,
	MeshDraw
};

class Renderer {
public:
	struct Resolution {
		std::uint64_t width;
		std::uint64_t height;
	};

	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;

	[[nodiscard]]
	virtual Resolution GetFirstDisplayCoordinates() const = 0;

	virtual void SetThreadPool(std::shared_ptr<IThreadPool> threadPoolArg) noexcept = 0;
	virtual void SetBackgroundColour(const std::array<float, 4>& colour) noexcept = 0;
	virtual void SetShaderPath(const wchar_t* path) noexcept = 0;
	virtual void SetSharedDataContainer(
		std::shared_ptr<ISharedDataContainer> sharedData
	) noexcept = 0;

	[[nodiscard]]
	virtual size_t AddTexture(
		std::unique_ptr<std::uint8_t> textureData, size_t width, size_t height
	) = 0; // Returns the index of the texture in its Resource Heap

	virtual void AddModelSet(
		std::vector<std::shared_ptr<IModel>>&& models, const std::wstring& pixelShader
	) = 0;
	virtual void AddMeshletModelSet(
		std::vector<MeshletModel>&& meshletModels, const std::wstring& pixelShader
	) = 0;
	virtual void AddModelInputs(
		std::vector<Vertex>&& gVertices, std::vector<std::uint32_t>&& gIndices
	) = 0;
	virtual void AddModelInputs(
		std::vector<Vertex>&& gVertices, std::vector<std::uint32_t>&& gVerticesIndices,
		std::vector<std::uint32_t>&& gPrimIndices
	) = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void WaitForAsyncTasks() = 0;
	virtual void ProcessData() = 0;
};
#endif
