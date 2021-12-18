#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <IModel.hpp>

class Model : public IModel {
public:
	void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept override;

	const std::vector<Ceres::Float32_3>& GetVertices() const noexcept override;
	const std::vector<std::uint16_t>& GetIndices() const noexcept override;
	const std::vector<VertexElementType>& GetVertexLayout() const noexcept override;

	Ceres::Matrix GetTransform() const noexcept override;
	Ceres::VectorF32 GetSolidColor() const noexcept override;

protected:
	std::vector<Ceres::Float32_3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	Ceres::VectorF32 m_solidColor;
	Ceres::Matrix m_transform;
};
#endif
