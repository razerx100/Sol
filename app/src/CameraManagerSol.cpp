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
