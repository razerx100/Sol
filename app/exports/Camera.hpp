#ifndef CAMERA_SOL_HPP_
#define CAMERA_SOL_HPP_
#include <cstdint>

#include <DirectXMath.h>

typedef DirectX::XMFLOAT4 Plane;

// Thanks to the relic that is Windows.h, there are two macros near and far and
// so we can't use those names. And they are used in the ComPtr header.
struct Frustum
{
	Plane leftP;
	Plane rightP;
	Plane bottomP;
	Plane topP;
	Plane nearP;
	Plane farP;
};

class Camera
{
public:
	virtual ~Camera() = default;

	[[nodiscard]]
	// The could probably change every frame, so we can just return it. As it is generated
	// by call to a different function, due to copy elision there won't be unnecessary copies.
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetCameraPosition() const noexcept = 0;
	// The projection matrix won't be updated much. So, returning it every frame and then copying
	// would result in an extra useless copy. So, it would be better to just pass the buffer pointer
	// and let this class make the copy. Also, makes sure address have enough so the size of
	// an XMMATRIX can be written.
	virtual void GetProjectionMatrix(void* address) const noexcept = 0;
	// No need to return the FOV, as that will be used to calculate the Projection matrix.
	[[nodiscard]]
	virtual Frustum GetViewFrustum() const noexcept = 0;
	[[nodiscard]]
	virtual Frustum GetViewFrustum(const DirectX::XMMATRIX& viewMatrix) const noexcept = 0;
};
#endif
