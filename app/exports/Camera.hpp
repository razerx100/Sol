#ifndef CAMERA_SOL_HPP_
#define CAMERA_SOL_HPP_
#include <cstdint>

#include <DirectXMath.h>

class Camera
{
public:
	virtual ~Camera() = default;

	[[nodiscard]]
	// The could probably change every frame, so we can just return it. As it is generated
	// by call to a different function, due to copy elision there won't be unnecessary copies.
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;
	[[nodiscard]]
	// The projection matrix won't be updated much. So, returning it every frame and then copying
	// would result in an extra useless copy. So, it would be better to just pass the buffer pointer
	// and let this class make the copy. Also, makes sure address have enough so the size of
	// an XMMATRIX can be written.
	virtual void GetProjectionMatrix(void* address) const noexcept = 0;
	// No need to return the FOV, as that will be used to calculate the Projection matrix.
};
#endif
