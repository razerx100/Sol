#ifndef CAMERA_MANAGER_SOL_HPP_
#define CAMERA_MANAGER_SOL_HPP_
#include <cstdint>
#include <vector>
#include <memory>
#include <concepts>

#include <DirectXMath.h>
#include <Camera.hpp>

namespace Sol
{
class BaseCamera
{
public:
	BaseCamera() : m_camera{} {}

	void SetCameraPosition(const DirectX::XMFLOAT3& position) noexcept
	{
		m_camera.SetCameraPosition(DirectX::XMLoadFloat3(&position));
	}

	[[nodiscard]]
	DirectX::XMFLOAT3 GetCameraPosition() const noexcept
	{
		return m_camera.GetCameraPosition();
	}
	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept
	{
		return m_camera.GetViewMatrix();
	}
	[[nodiscard]]
	Frustum GetViewFrustum() const noexcept
	{
		return GetViewFrustum(GetViewMatrix());
	}

	[[nodiscard]]
	Frustum GetViewFrustum(const DirectX::XMMATRIX& viewMatrix) const noexcept
	{
		return m_camera.GetViewFrustum(viewMatrix);
	}

	void GetProjectionMatrix(void* address) const noexcept
	{
		m_camera.GetProjectionMatrix(address);
	}

	[[nodiscard]]
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept
	{
		return m_camera.GetProjectionMatrix();
	}

	[[nodiscard]]
	const Camera& GetCamera() const noexcept { return m_camera; }

protected:
	Camera m_camera;
};

class PerspectiveCamera : public BaseCamera
{
public:
	PerspectiveCamera();

	PerspectiveCamera& SetWorldForwardDirection(bool positiveZ) noexcept;

	void SetTravelSpeed(float travelSpeed) noexcept { m_travelSpeed = travelSpeed; }
	void SetRotationSpeed(float rotationSpeed) noexcept { m_rotationSpeed = rotationSpeed; }
	void SetFOV(float fovDegree) noexcept { m_fovRadian = DirectX::XMConvertToRadians(fovDegree); }

	void SetProjectionMatrix(std::uint32_t width, std::uint32_t height) noexcept;

	[[nodiscard]]
	float GetFOVRadian() const noexcept { return m_fovRadian; }
	[[nodiscard]]
	float GetFOVDegree() const noexcept { return DirectX::XMConvertToDegrees(m_fovRadian); }

	void CalculateViewMatrix() noexcept;

protected:
	DirectX::XMVECTOR m_worldForwardDirection;
	DirectX::XMVECTOR m_cameraFocusDirection;
	DirectX::XMVECTOR m_upVector;
	float             m_travelSpeed;
	float             m_rotationSpeed;
	float             m_fovRadian;
};

class PerspectiveCameraEuler : public PerspectiveCamera
{
public:
	PerspectiveCameraEuler() : m_pitch{ 0.f }, m_yaw{ 0.f } {}

	void MoveLeft(float offset) noexcept
	{
		MoveX(-offset);
	}
	void MoveRight(float offset) noexcept
	{
		MoveX(offset);
	}
	void MoveUp(float offset) noexcept
	{
		MoveY(offset);
	}
	void MoveDown(float offset) noexcept
	{
		MoveY(-offset);
	}
	void MoveForward(float offset) noexcept
	{
		MoveZ(offset);
	}
	void MoveBackward(float offset) noexcept
	{
		MoveZ(-offset);
	}

	void LookAround(float offsetX, float offsetY) noexcept;

private:
	void CalculateTranslation(const DirectX::XMVECTOR& offsetVector) noexcept;

private:
	void MoveX(float offset) noexcept
	{
		CalculateTranslation(DirectX::XMVectorSet(offset, 0.f, 0.f, 0.f));
	}
	void MoveY(float offset) noexcept
	{
		CalculateTranslation(DirectX::XMVectorSet(0.f, offset, 0.f, 0.f));
	}
	void MoveZ(float offset) noexcept
	{
		CalculateTranslation(DirectX::XMVectorSet(0.f, 0.f, offset, 0.f));
	}

private:
	float m_pitch;
	float m_yaw;
};

class CameraManager
{
	using PerspectiveCamera_t = PerspectiveCameraEuler;

public:
	CameraManager() : m_perspectiveCameras{} {}

	[[nodiscard]]
	std::uint32_t AddPerspectiveCamera() noexcept
	{
		const auto cameraIndex = static_cast<std::uint32_t>(std::size(m_perspectiveCameras));

		m_perspectiveCameras.emplace_back();

		return cameraIndex;
	}

	[[nodiscard]]
	auto&& GetPerspectiveCamera(this auto&& self, size_t index) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_perspectiveCameras[index]);
	}

private:
	std::vector<PerspectiveCamera_t> m_perspectiveCameras;
};
}
#endif
