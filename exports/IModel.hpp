#ifndef __I_MODEL_HPP__
#define __I_MODEL_HPP__
#include <cstdint>
#include <CRSVector.hpp>
#include <CRSMatrix.hpp>
#include <vector>

enum class VertexElementType {
	Position,
	Color
};

class IModel {
public:
	virtual ~IModel() = default;

	[[nodiscard]]
	virtual const std::vector<Ceres::Float32_3>& GetVertices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::uint16_t>& GetIndices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<VertexElementType>& GetVertexLayout() const noexcept = 0;

	[[nodiscard]]
	virtual Ceres::Matrix GetTransform() const noexcept = 0;
	[[nodiscard]]
	virtual Ceres::VectorF32 GetSolidColor() const noexcept = 0;
};
#endif
