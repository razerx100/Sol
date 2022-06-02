#ifndef CAMERA_MANAGER_SOL_HPP_
#define CAMERA_MANAGER_SOL_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <concepts>

#include <DirectXMath.h>

class _BaseCamera {
public:
	virtual ~_BaseCamera() = default;

	void SetCameraPosition(const DirectX::XMFLOAT3& cameraPostion) noexcept;

	[[nodiscard]]
	DirectX::XMFLOAT3 GetCameraPosition() const noexcept;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;

protected:
	DirectX::XMVECTOR m_cameraPosition;
};

class PerspectiveCamera : public _BaseCamera {
public:
	PerspectiveCamera() noexcept;

	void SetWorldForwardDirection(bool positiveZ) noexcept;
	void SetTravelSpeed(float travelSpeed) noexcept;
	void SetRotationSpeed(float rotationSpeed) noexcept;

	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept final;

protected:
	DirectX::XMVECTOR m_worldForwardDirection;
	DirectX::XMVECTOR m_cameraFocusDirection;
	DirectX::XMVECTOR m_upVector;
	float m_travelSpeed;
	float m_rotationSpeed;
};

class PerspectiveCameraEuler final : public PerspectiveCamera {
public:
	PerspectiveCameraEuler() noexcept;

	void MoveLeft(float deltaTime) noexcept;
	void MoveRight(float deltaTime) noexcept;
	void MoveUp(float deltaTime) noexcept;
	void MoveDown(float deltaTime) noexcept;
	void MoveForward(float deltaTime) noexcept;
	void MoveBackward(float deltaTime) noexcept;

	void LookAround(float offsetX, float offsetY) noexcept;

private:
	void MoveCamera(DirectX::XMVECTOR& translateVector) noexcept;

private:
	float m_pitch;
	float m_yaw;
};

class CameraManagerSol {
public:
	CameraManagerSol() noexcept;

	[[nodiscard]]
	size_t AddEulerCameraAndGetIndex(std::unique_ptr<PerspectiveCameraEuler> camera) noexcept;

	void SetCurrentCameraIndex(size_t cameraIndex);
	void SetCamera() const noexcept;

	[[nodiscard]]
	PerspectiveCameraEuler* GetEulerCamera(size_t cameraIndex) const;

private:
	void CheckIndexValidity(size_t index) const;

private:
	std::vector<std::unique_ptr<_BaseCamera>> m_cameras;
	size_t m_currentCameraIndex;
};
#endif
