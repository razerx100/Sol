#include <SharedDataContainer.hpp>

SharedDataContainer::SharedDataContainer()
	: m_viewMatrix{}, m_fov(65) {}

void SharedDataContainer::SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) noexcept {
	m_viewMatrix = viewMatrix;
}

DirectX::XMMATRIX SharedDataContainer::GetViewMatrix() const noexcept {
	return m_viewMatrix;
}

void SharedDataContainer::SetFov(std::uint32_t fovAngleInDegree) noexcept {
	m_fov = fovAngleInDegree;
}

std::uint32_t SharedDataContainer::GetFov() const noexcept {
	return m_fov;
}
