#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <IModel.hpp>
#include <CRSVector.hpp>

class Model : public IModel {
public:
	void SetSolidColor(const float* const colorVector) noexcept override;

	void GetVertices(
		float* vertices, std::uint32_t& vertexCount
	) const noexcept override;
	void GetIndices(
		std::uint16_t* indices, std::uint32_t& indexCount
	) const noexcept override;

	void GetTransform(float* matrix) const noexcept override;
	void GetSolidColor(float* colorVector) const noexcept override;


protected:
	std::vector<Ceres::Float32_3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	Ceres::VectorF32 m_solidColor;
};
#endif
