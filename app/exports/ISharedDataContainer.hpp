#ifndef I_SHARED_DATA_CONTAINER_HPP_
#define I_SHARED_DATA_CONTAINER_HPP_
#include <cstdint>

#include <DirectXMath.h>

class ISharedDataContainer {
public:
	virtual ~ISharedDataContainer() = default;

	virtual void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;

	virtual void SetFov(std::uint32_t fovAngleInDegree) noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetFov() const noexcept = 0;
};
#endif
