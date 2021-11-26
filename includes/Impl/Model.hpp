#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <IModel.hpp>

class Model : public IModel {
public:
	void SetSolidColor(const DirectX::XMVECTORF32& color) noexcept override;

	void GetVertices(
		DirectX::XMFLOAT3* vertices, std::uint32_t& vertexCount
	) const noexcept override;
	void GetIndices(
		std::uint16_t* indices, std::uint32_t& indexCount
	) const noexcept override;

	DirectX::XMMATRIX GetTransform() const noexcept override;
	DirectX::XMVECTORF32 GetSolidColor() const noexcept override;

protected:
	std::vector<DirectX::XMFLOAT3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	DirectX::XMMATRIX m_transform;
	DirectX::XMVECTORF32 m_solidColor;
};
#endif
