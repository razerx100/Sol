#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <ISolModel.hpp>

class Model : public ISolModel {
public:
	void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept override;
	void ResetVerticesAndIndices() noexcept override;

	[[nodiscard]]
	const std::vector<Ceres::Float32_3>& GetVertices() const noexcept override;
	[[nodiscard]]
	const std::vector<std::uint16_t>& GetIndices() const noexcept override;
	[[nodiscard]]
	const std::vector<VertexElementType>& GetVertexLayout() const noexcept override;

	[[nodiscard]]
	Ceres::Matrix GetTransform() const noexcept override;
	[[nodiscard]]
	Ceres::VectorF32 GetSolidColor() const noexcept override;

protected:
	std::vector<Ceres::Float32_3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	Ceres::VectorF32 m_solidColor;
	Ceres::Matrix m_transform;
};
#endif
