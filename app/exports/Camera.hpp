#ifndef CAMERA_SOL_HPP_
#define CAMERA_SOL_HPP_
#include <cstdint>

#include <DirectXMath.h>

class Camera
{
public:
	[[nodiscard]]
	virtual std::uint32_t GetFov() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetProjectionMatrix() const noexcept = 0;
};
#endif
