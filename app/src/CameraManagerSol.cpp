#include <cmath>
#include <algorithm>
#include <SolThrowMacros.hpp>

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

// Camera Manager
CameraManagerSol::CameraManagerSol() noexcept
	: m_currentCameraIndex(0u) {}

size_t CameraManagerSol::AddEulerCameraAndGetIndex(
	std::unique_ptr<PerspectiveCameraEuler> camera
) noexcept {
	m_cameras.emplace_back(std::move(camera));

	return std::size(m_cameras) - 1u;
}

void CameraManagerSol::SetCurrentCameraIndex(size_t cameraIndex) {
	assert(std::size(m_cameras) > cameraIndex && "There is no camera at the specified index.");

	m_currentCameraIndex = cameraIndex;
}

void CameraManagerSol::SetCamera() const noexcept {
	if (!std::empty(m_cameras))
		Sol::sharedData->SetViewMatrix(
			m_cameras.at(m_currentCameraIndex)->GetViewMatrix()
		);
}

PerspectiveCameraEuler* CameraManagerSol::GetEulerCamera(size_t cameraIndex) const {
	assert(std::size(m_cameras) > cameraIndex && "There is no camera at the specified index.");

	PerspectiveCameraEuler* camera =
		dynamic_cast<PerspectiveCameraEuler*>(m_cameras.at(cameraIndex).get());

	assert(camera && "Camera at the specified index isn't a Euler Camera.");

	return camera;
}

PerspectiveCameraEuler* CameraManagerSol::GetFirstEulerCamera() const {
	return GetEulerCamera(0u);
}
