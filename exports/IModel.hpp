#ifndef __I_MODEL_HPP__
#define __I_MODEL_HPP__
#include <cstdint>
#include <CRSVector.hpp>
#include <CRSMatrix.hpp>

class IModel {
public:
	virtual ~IModel() = default;

	virtual void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept = 0;

	virtual void GetVertices(
		Ceres::Float32_3* vertices, std::uint32_t& vertexCount
	) const noexcept = 0;
	virtual void GetIndices(
		std::uint16_t* indices, std::uint32_t& indexCount
	) const noexcept = 0;

	virtual void GetTransform(Ceres::Matrix& transform) const noexcept = 0;
	virtual void GetSolidColor(Ceres::VectorF32& colorVector) const noexcept = 0;
};
#endif
