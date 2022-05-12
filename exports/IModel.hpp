#ifndef I_MODEL_HPP_
#define I_MODEL_HPP_
#include <cstdint>
#include <vector>

namespace DirectX {
#ifdef _XM_NO_INTRINSICS_
	struct XMMATRIX;
#else
	struct alignas(16) XMMATRIX;
#endif
}

// Include DirectXMath in source files

enum class VertexElementType {
	Position,
	UV
};

struct TextureData {
	std::uint32_t uStart;
	std::uint32_t uEnd;
	std::uint32_t uMax;
	std::uint32_t vStart;
	std::uint32_t vEnd;
	std::uint32_t vMax;
};

class IModel {
public:
	virtual ~IModel() = default;

	[[nodiscard]]
	virtual const void* GetVertexData() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetVertexStrideSize() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetVertexBufferSize() const noexcept = 0;
	[[nodiscard]]
	virtual const void* GetIndexData() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetIndexBufferSize() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetIndexCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetTextureIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<VertexElementType>& GetVertexLayout() const noexcept = 0;
	[[nodiscard]]
	virtual const TextureData& GetTextureInfo() const noexcept = 0;

	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
};
#endif
