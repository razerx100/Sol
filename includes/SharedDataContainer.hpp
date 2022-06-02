#ifndef SHARED_DATA_CONTAINER_HPP_
#define SHARED_DATA_CONTAINER_HPP_
#include <ISharedDataContainer.hpp>

class SharedDataContainer final : public ISharedDataContainer {
public:
	SharedDataContainer();

	void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) noexcept override;
	[[nodiscard]]
	DirectX::XMMATRIX GetViewMatrix() const noexcept override;

	void SetFov(std::uint32_t fovAngleInDegree) noexcept override;
	[[nodiscard]]
	std::uint32_t GetFov() const noexcept override;

private:
	DirectX::XMMATRIX m_viewMatrix;
	std::uint32_t m_fov;
};
#endif
