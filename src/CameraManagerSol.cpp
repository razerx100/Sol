#include <cmath>
#include <algorithm>
#include <SolThrowMacros.hpp>

#include <CameraManagerSol.hpp>
#include <Sol.hpp>

using namespace DirectX;

// Base Camera

XMFLOAT3 _BaseCamera::GetCameraPosition() const noexcept {
	XMFLOAT3 value = {};
	XMStoreFloat3(&value, m_cameraPosition);

	return value;
}

void _BaseCamera::SetCameraPosition(const XMFLOAT3& cameraPosition) noexcept {
	m_cameraPosition = XMLoadFloat3(&cameraPosition);
}

// Perspective Camera
PerspectiveCamera::PerspectiveCamera() noexcept
	: m_worldForwardDirection(XMVectorSet(0.f, 0.f, 1.f, 0.f)),
	m_cameraFocusDirection(m_worldForwardDirection),
	m_upVector(XMVectorSet(0.f, 1.f, 0.f, 0.f)), m_travelSpeed(5.f), m_rotationSpeed(5.f) {}

PerspectiveCamera& PerspectiveCamera::SetWorldForwardDirection(bool positiveZ) noexcept {
	if (positiveZ)
		m_worldForwardDirection = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	else
		m_worldForwardDirection = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	m_cameraFocusDirection = m_worldForwardDirection;

	return *this;
}

void PerspectiveCamera::SetTravelSpeed(float travelSpeed) noexcept {
	m_travelSpeed = travelSpeed;
}

void PerspectiveCamera::SetRotationSpeed(float rotationSpeed) noexcept {
	m_rotationSpeed = rotationSpeed;
}

XMMATRIX PerspectiveCamera::GetViewMatrix() const noexcept {
	return XMMatrixLookAtLH(
		m_cameraPosition,
		m_cameraPosition + m_cameraFocusDirection,
		m_upVector
	);
}

// Perspective Camera with Euler's Rotation
PerspectiveCameraEuler::PerspectiveCameraEuler() noexcept
	: m_translateVector{ XMVectorSet(0.f, 0.f, 0.f, 0.f) }, m_pitch{ 0.f }, m_yaw{ 0.f } {}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveLeft(float offset) noexcept {
	MoveX(-offset);

	return *this;
}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveRight(float offset) noexcept {
	MoveX(offset);

	return *this;
}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveUp(float offset) noexcept {
	MoveY(offset);

	return *this;
}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveDown(float offset) noexcept {
	MoveY(-offset);

	return *this;
}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveForward(float offset) noexcept {
	MoveZ(offset);

	return *this;
}

PerspectiveCameraEuler& PerspectiveCameraEuler::MoveBackward(float offset) noexcept {
	MoveZ(-offset);

	return *this;
}

void PerspectiveCameraEuler::MoveX(float offset) noexcept {
	m_translateVector = XMVectorSet(offset, 0.f, 0.f, 0.f);
}

void PerspectiveCameraEuler::MoveY(float offset) noexcept {
	m_translateVector = XMVectorSet(0.f, offset, 0.f, 0.f);
}

void PerspectiveCameraEuler::MoveZ(float offset) noexcept {
	m_translateVector = XMVectorSet(0.f, 0.f, offset, 0.f);
}

void PerspectiveCameraEuler::MoveCamera() noexcept {
	m_translateVector = XMVector3Transform(
		m_translateVector,
		XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f) *
		XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)
	);

	m_cameraPosition += m_translateVector;
}

void PerspectiveCameraEuler::LookAround(float offsetX, float offsetY) noexcept {
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
