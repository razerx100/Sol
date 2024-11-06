#include <cmath>
#include <algorithm>

#include <CameraManagerSol.hpp>
#include <Sol.hpp>

using namespace DirectX;

// Base Camera
XMFLOAT3 _BaseCamera::GetCameraPosition() const noexcept
{
	XMFLOAT3 value = {};
	XMStoreFloat3(&value, m_cameraPosition);

	return value;
}

void _BaseCamera::SetProjectionMatrix(std::uint32_t width, std::uint32_t height) noexcept
{
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		m_fovRadian,
		static_cast<float>(width) / height,
		0.1f, 100.f
	);
}

void _BaseCamera::GetProjectionMatrix(void* address) const noexcept
{
	memcpy(address, &m_projectionMatrix, sizeof(XMMATRIX));
}

DirectX::XMFLOAT4 _BaseCamera::GetFloat4(const DirectX::XMVECTOR& vector) noexcept
{
	DirectX::XMFLOAT4 float4{};

	DirectX::XMStoreFloat4(&float4, vector);

	return float4;
}

Frustum _BaseCamera::GetViewFrustum() const noexcept
{
	return GetViewFrustum(GetViewMatrix());
}

Frustum _BaseCamera::GetViewFrustum(const DirectX::XMMATRIX& viewMatrix) const noexcept
{
	using namespace DirectX;

	// The projection matrix projects vertices onto the clip space. And multipling the view
	// matrix change into a matrix which projects vertices from their local space to the clip space.
	// So, in these two transforms the x, y, z, w planes are in their columns, as then they would
	// be correctly multipled with a vector. And we can't really access a column from an XMMATRIX,
	// so we are transposing it.

	const XMMATRIX viewPMatrix = XMMatrixTranspose(viewMatrix * m_projectionMatrix);

	// In D3D/Vulkan, the clip space extents to -w, -w, 0 to w, w, w. So, the near
	// plane is on the z plane not w + z.
	Frustum viewFrustm
	{
		.leftP   = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] + viewPMatrix.r[0])),
		.rightP  = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[0])),
		.bottomP = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] + viewPMatrix.r[1])),
		.topP    = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[1])),
		.nearP   = GetFloat4(XMPlaneNormalize(viewPMatrix.r[2])),
		.farP    = GetFloat4(XMPlaneNormalize(viewPMatrix.r[3] - viewPMatrix.r[2]))
	};

	return viewFrustm;
}

// Perspective Camera
PerspectiveCamera::PerspectiveCamera()
	: m_worldForwardDirection{ XMVectorSet(0.f, 0.f, 1.f, 0.f) },
	m_cameraFocusDirection{ m_worldForwardDirection },
	m_upVector{ XMVectorSet(0.f, 1.f, 0.f, 0.f) }, m_travelSpeed{ 5.f }, m_rotationSpeed{ 5.f }
{}

PerspectiveCamera& PerspectiveCamera::SetWorldForwardDirection(bool positiveZ) noexcept
{
	if (positiveZ)
		m_worldForwardDirection = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	else
		m_worldForwardDirection = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	m_cameraFocusDirection = m_worldForwardDirection;

	return *this;
}

XMMATRIX PerspectiveCamera::GetViewMatrix() const noexcept
{
	return XMMatrixLookAtLH(
		m_cameraPosition,
		m_cameraPosition + m_cameraFocusDirection,
		m_upVector
	);
}

// Perspective Camera with Euler's Rotation
void PerspectiveCameraEuler::MoveCamera() noexcept
{
	m_translateVector = XMVector3Transform(
		m_translateVector,
		XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f) *
		XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)
	);

	m_cameraPosition += m_translateVector;
}

void PerspectiveCameraEuler::LookAround(float offsetX, float offsetY) noexcept
{
	// Keep yaw value below 360 degree
	m_yaw = std::fmod(m_yaw + offsetX * m_rotationSpeed, XM_2PI);

	// Keep the pitch angle between 89 and -89 degree
	m_pitch = std::clamp(
		m_pitch + offsetY * m_rotationSpeed,
		XMConvertToRadians(-89.f), XMConvertToRadians(89.f)
	);

	m_cameraFocusDirection = XMVector3Transform(
		m_worldForwardDirection,
		XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f)
	);
}
