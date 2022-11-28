#ifndef I_MODEL_HPP_
#define I_MODEL_HPP_
#include <cstdint>
#include <array>

#include <DirectXMath.h>

struct UVInfo {
	float uOffset;
	float vOffset;
	float uRatio;
	float vRatio;
};

struct ModelBounds {
	DirectX::XMFLOAT3 positiveAxes;
	DirectX::XMFLOAT3 negativeAxes;
};

class IModel {
public:
	virtual ~IModel() = default;

	[[nodiscard]]
	virtual std::uint32_t GetIndexCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetIndexOffset() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetTextureIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetUVInfo() const noexcept = 0;

	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetModelOffset() const noexcept = 0;
	[[nodiscard]]
	virtual ModelBounds GetBoundingBox() const noexcept = 0;
};
#endif
