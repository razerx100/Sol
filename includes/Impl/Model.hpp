#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <IModel.hpp>

class Model : public IModel {
public:
	void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept override;

	void GetVertices(
		Ceres::Float32_3* vertices, std::uint32_t& vertexCount
	) const noexcept override;
	void GetIndices(
		std::uint16_t* indices, std::uint32_t& indexCount
	) const noexcept override;

	void GetTransform(Ceres::Matrix& transform) const noexcept override;
	void GetSolidColor(Ceres::VectorF32& colorVector) const noexcept override;


protected:
	std::vector<Ceres::Float32_3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	Ceres::VectorF32 m_solidColor;
	Ceres::Matrix m_transform;
};
#endif
