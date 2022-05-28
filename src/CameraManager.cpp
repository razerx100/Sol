#include <CameraManager.hpp>

// Perspective Camera

PerspectiveCamera::PerspectiveCamera() noexcept
	: m_cameraPosition{}, m_focusPosition{}, m_upVector{ 0.f, 1.f, 0.f } {}

void PerspectiveCamera::SetFocusPosition(const DirectX::XMFLOAT3& focusPosition) noexcept {
	m_focusPosition = focusPosition;
}

void PerspectiveCamera::SetCameraPosition(const DirectX::XMFLOAT3& cameraPosition) noexcept {
	m_cameraPosition = cameraPosition;
}

void PerspectiveCamera::SetCameraX(float cameraX) noexcept {
	m_cameraPosition.x = cameraX;
}

void PerspectiveCamera::SetCameraY(float cameraY) noexcept {
	m_cameraPosition.y = cameraY;
}

void PerspectiveCamera::SetCameraZ(float cameraZ) noexcept {
	m_cameraPosition.z = cameraZ;
}

DirectX::XMFLOAT3 PerspectiveCamera::GetCameraPosition() const noexcept {
	return m_cameraPosition;
}

DirectX::XMMATRIX PerspectiveCamera::GetViewMatrix() const noexcept {
	return DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&m_cameraPosition),
		DirectX::XMLoadFloat3(&m_focusPosition),
		DirectX::XMLoadFloat3(&m_upVector)
	);
}

// Camera Manager
size_t CameraManager::AddPerspectiveCameraAndGetIndex(const PerspectiveCamera& camera) noexcept {
	m_perspectiveCameras.emplace_back(camera);

	return std::size(m_perspectiveCameras) - 1u;
}

void CameraManager::UpdatePerspectiveCameraByIndex(
	size_t index, const PerspectiveCamera& camera
) noexcept {
	m_perspectiveCameras[index] = camera;
}

PerspectiveCamera CameraManager::GetPerspectiveCameraByIndex(size_t index) const noexcept {
	return m_perspectiveCameras[index];
}
