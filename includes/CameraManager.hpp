#ifndef CAMERA_MANAGER_HPP_
#define CAMERA_MANAGER_HPP_
#include <cstdint>
#include <vector>

#include <DirectXMath.h>

class PerspectiveCamera {
public:
	PerspectiveCamera() noexcept;

	void SetFocusPosition(const DirectX::XMFLOAT3& focusPosition) noexcept;
	void SetCameraPosition(const DirectX::XMFLOAT3& cameraPostion) noexcept;

	void SetCameraX(float cameraX) noexcept;
	void SetCameraY(float cameraY) noexcept;
	void SetCameraZ(float cameraZ) noexcept;

	[[nodiscard]]
	DirectX::XMFLOAT3 GetCameraPosition() const noexcept;
	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept;

private:
	DirectX::XMFLOAT3 m_cameraPosition;
	DirectX::XMFLOAT3 m_focusPosition;
	DirectX::XMFLOAT3 m_upVector;
};

class CameraManager {
public:
	[[nodiscard]]
	size_t AddPerspectiveCameraAndGetIndex(const PerspectiveCamera& camera) noexcept;

	void UpdatePerspectiveCameraByIndex(
		size_t index, const PerspectiveCamera& camera
	) noexcept;

	[[nodiscard]]
	PerspectiveCamera GetPerspectiveCameraByIndex(size_t index) const noexcept;

private:
	std::vector<PerspectiveCamera> m_perspectiveCameras;
};
#endif
