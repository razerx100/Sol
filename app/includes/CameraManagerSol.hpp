#ifndef CAMERA_MANAGER_SOL_HPP_
#define CAMERA_MANAGER_SOL_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <concepts>

#include <DirectXMath.h>
#include <Camera.hpp>

class _BaseCamera : public Camera
{
public:
	_BaseCamera()
		: m_fovRadian{ DirectX::XMConvertToRadians(65.f) },
		m_cameraPosition{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f) }, m_projectionMatrix{}
	{}

	virtual ~_BaseCamera() = default;

	void SetCameraPosition(const DirectX::XMFLOAT3& cameraPosition) noexcept
	{
		m_cameraPosition = XMLoadFloat3(&cameraPosition);
	}
	void SetFov(float fovAngleInDegree) noexcept
	{
		m_fovRadian = DirectX::XMConvertToRadians(fovAngleInDegree);
	};
	// Should be called after a resize.
	void SetProjectionMatrix(std::uint32_t width, std::uint32_t height) noexcept;

	[[nodiscard]]
	float GetFovRadian() const noexcept { return m_fovRadian; }
	[[nodiscard]]
	float GetFovDegrees() const noexcept { return DirectX::XMConvertToDegrees(m_fovRadian); }

	[[nodiscard]]
	DirectX::XMFLOAT3 GetCameraPosition() const noexcept;
	[[nodiscard]]
	virtual DirectX::XMMATRIX GetViewMatrix() const noexcept = 0;

	void GetProjectionMatrix(void* address) const noexcept override;

protected:
	float             m_fovRadian;
	float             padding;
	DirectX::XMVECTOR m_cameraPosition;
	DirectX::XMMATRIX m_projectionMatrix;
};

class PerspectiveCamera : public _BaseCamera
{
public:
	PerspectiveCamera();

	PerspectiveCamera& SetWorldForwardDirection(bool positiveZ) noexcept;
	void SetTravelSpeed(float travelSpeed) noexcept { m_travelSpeed = travelSpeed; }
	void SetRotationSpeed(float rotationSpeed) noexcept { m_rotationSpeed = rotationSpeed; }

	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept final;

protected:
	DirectX::XMVECTOR m_worldForwardDirection;
	DirectX::XMVECTOR m_cameraFocusDirection;
	DirectX::XMVECTOR m_upVector;
	float             m_travelSpeed;
	float             m_rotationSpeed;
};

class PerspectiveCameraEuler final : public PerspectiveCamera
{
public:
	PerspectiveCameraEuler()
		: m_translateVector{ DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f) }, m_pitch{ 0.f }, m_yaw{ 0.f }
	{}

	[[nodiscard]]
	PerspectiveCameraEuler& MoveLeft(float offset) noexcept
	{
		MoveX(-offset);

		return *this;
	}
	[[nodiscard]]
	PerspectiveCameraEuler& MoveRight(float offset) noexcept
	{
		MoveX(offset);

		return *this;
	}
	[[nodiscard]]
	PerspectiveCameraEuler& MoveUp(float offset) noexcept
	{
		MoveY(offset);

		return *this;
	}
	[[nodiscard]]
	PerspectiveCameraEuler& MoveDown(float offset) noexcept
	{
		MoveY(-offset);

		return *this;
	}
	[[nodiscard]]
	PerspectiveCameraEuler& MoveForward(float offset) noexcept
	{
		MoveZ(offset);

		return *this;
	}
	[[nodiscard]]
	PerspectiveCameraEuler& MoveBackward(float offset) noexcept
	{
		MoveZ(-offset);

		return *this;
	}

	void MoveCamera() noexcept;

	void LookAround(float offsetX, float offsetY) noexcept;

private:
	void MoveX(float offset) noexcept
	{
		m_translateVector = DirectX::XMVectorSet(offset, 0.f, 0.f, 0.f);
	}
	void MoveY(float offset) noexcept
	{
		m_translateVector = DirectX::XMVectorSet(0.f, offset, 0.f, 0.f);
	}
	void MoveZ(float offset) noexcept
	{
		m_translateVector = DirectX::XMVectorSet(0.f, 0.f, offset, 0.f);
	}

private:
	DirectX::XMVECTOR m_translateVector;
	float             m_pitch;
	float             m_yaw;
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
	[[nodiscard]]
	PerspectiveCameraEuler* GetFirstEulerCamera() const;

private:
	std::vector<std::unique_ptr<_BaseCamera>> m_cameras;
	size_t m_currentCameraIndex;
};
#endif
