#ifndef I_MODEL_HPP_
#define I_MODEL_HPP_
#include <cstdint>
#include <vector>
#include <memory>

#include <DirectXMath.h>

struct UVInfo {
	float uOffset;
	float vOffset;
	float uRatio;
	float vRatio;
};

class IModel {
public:
	virtual ~IModel() = default;

	[[nodiscard]]
	virtual std::unique_ptr<std::uint8_t> GetVertexData() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetVertexStrideSize() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetVertexBufferSize() const noexcept = 0;
	[[nodiscard]]
	virtual std::unique_ptr<std::uint8_t> GetIndexData() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetIndexBufferSize() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetIndexCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetTextureIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetUVInfo() const noexcept = 0;

	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
};
#endif
