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
	Camera()
		: m_cameraPosition{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f) },
		m_projectionMatrix{ DirectX::XMMatrixIdentity() },
		m_viewMatrix{ DirectX::XMMatrixIdentity() }
	{}

	void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) noexcept
	{
		m_viewMatrix = viewMatrix;
	}

	void SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix) noexcept
	{
		m_projectionMatrix = projectionMatrix;
	}

	void SetCameraPosition(const DirectX::XMVECTOR& cameraPosition) noexcept
	{
		m_cameraPosition = cameraPosition;
	}

	// The could probably change every frame, so we can just return it. As it is generated
	// by call to a different function, due to copy elision there won't be unnecessary copies.
	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept { return m_viewMatrix; }
	[[nodiscard]]
	DirectX::XMFLOAT3 GetCameraPosition() const noexcept
	{
		DirectX::XMFLOAT3 position{};

		DirectX::XMStoreFloat3(&position, m_cameraPosition);

		return position;
	}
	[[nodiscard]]
	const DirectX::XMVECTOR& GetCameraPositionVector() const noexcept
	{
		return m_cameraPosition;
	}

	// The projection matrix won't be updated much. So, returning it every frame and then copying
	// would result in an extra useless copy. So, it would be better to just pass the buffer pointer
	// and let this class make the copy. Also, makes sure address have enough so the size of
	// an XMMATRIX can be written.
	void GetProjectionMatrix(void* address) const noexcept
	{
		memcpy(address, &m_projectionMatrix, sizeof(DirectX::XMMATRIX));
	}

	[[nodiscard]]
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept
	{
		return m_projectionMatrix;
	}

	// No need to return the FOV, as that will be used to calculate the Projection matrix.
	[[nodiscard]]
	Frustum GetViewFrustum(const DirectX::XMMATRIX& viewMatrix) const noexcept
	{
		using namespace DirectX;

		// When the view matrix is transformed by the projection matrix, we can derive
		// the planes from the resulting matrix. A view matrix represents the x, y, z
		// normals of the camera in the first, second and the third column and the
		// camera location on the 4th column. The projection matrix adds the frustum limit
		// anything outside which gets clipped. And we can derive the planes from the
		// - w <= x <= w, -w <= y <= w, 0 <= z <= w equations. And the planes would be
		// in the world space.
		// Since we can't access the columns as a single vector here, we are transposing
		// it first to access them as a row.
		const XMMATRIX viewPMatrix = XMMatrixTranspose(viewMatrix * m_projectionMatrix);

		// In D3D/Vulkan, the clip space extents to -w, -w, 0 to w, w, w. So, the near
		// plane is on the z plane not w + z.
		return Frustum
		{
			.leftP   = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] + viewPMatrix.r[0])),
			.rightP  = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[0])),
			.bottomP = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] + viewPMatrix.r[1])),
			.topP    = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[1])),
			.nearP   = GetFloat4(XMPlaneNormalize(viewPMatrix.r[2])),
			.farP    = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[2]))
		};
	}

private:
	[[nodiscard]]
	static DirectX::XMFLOAT4 GetFloat4(const DirectX::XMVECTOR& vector) noexcept
	{
		DirectX::XMFLOAT4 float4{};

		DirectX::XMStoreFloat4(&float4, vector);

		return float4;
	}

private:
	DirectX::XMVECTOR m_cameraPosition;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_viewMatrix;
};
#endif
