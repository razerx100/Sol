#include <cmath>
#include <algorithm>

#include <CameraManagerSol.hpp>

namespace Sol
{
// Perspective Camera
PerspectiveCamera::PerspectiveCamera()
	: BaseCamera{},
	m_worldForwardDirection{ DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f) },
	m_cameraFocusDirection{ m_worldForwardDirection },
	m_upVector{ DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f) },
	m_travelSpeed{ 5.f }, m_rotationSpeed{ 5.f },
	m_fovRadian{ DirectX::XMConvertToRadians(65.f) }
{}

PerspectiveCamera& PerspectiveCamera::SetWorldForwardDirection(bool positiveZ) noexcept
{
	if (positiveZ)
		m_worldForwardDirection = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
	else
		m_worldForwardDirection = DirectX::XMVectorSet(0.f, 0.f, -1.f, 0.f);

	m_cameraFocusDirection = m_worldForwardDirection;

	return *this;
}

void PerspectiveCamera::SetProjectionMatrix(std::uint32_t width, std::uint32_t height) noexcept
{
	m_camera.SetProjectionMatrix(
		DirectX::XMMatrixPerspectiveFovLH(
			m_fovRadian, static_cast<float>(width) / height, 0.1f, 100.f
		)
	);
}

void PerspectiveCamera::CalculateViewMatrix() noexcept
{
	using namespace DirectX;

	const XMVECTOR& cameraPosition = m_camera.GetCameraPositionVector();

	m_camera.SetViewMatrix(
		XMMatrixLookAtLH(
			cameraPosition, cameraPosition + m_cameraFocusDirection, m_upVector
		)
	);
}

// Perspective Camera with Euler's Rotation
void PerspectiveCameraEuler::CalculateTranslation(const DirectX::XMVECTOR& offsetVector) noexcept
{
	using namespace DirectX;

	XMVECTOR transformedOffset = DirectX::XMVector3Transform(
		offsetVector,
		XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f) *
		XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)
	);

	const XMVECTOR& cameraPosition = m_camera.GetCameraPositionVector();

	m_camera.SetCameraPosition(cameraPosition + transformedOffset);
}

void PerspectiveCameraEuler::LookAround(float offsetX, float offsetY) noexcept
{
	// Keep yaw value below 360 degree
	m_yaw = std::fmod(m_yaw + offsetX * m_rotationSpeed, DirectX::XM_2PI);

	// Keep the pitch angle between 89 and -89 degree
	m_pitch = std::clamp(
		m_pitch + offsetY * m_rotationSpeed,
		DirectX::XMConvertToRadians(-89.f), DirectX::XMConvertToRadians(89.f)
	);

	m_cameraFocusDirection = DirectX::XMVector3Transform(
		m_worldForwardDirection,
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f)
	);
}
}
