#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <ISolModel.hpp>

struct Vertex {
	Ceres::Float32_3 position;
	Ceres::Float32_4 color;
};

class Model : public ISolModel {
public:
	void SetSolidColor(const Ceres::VectorF32& colorVector) noexcept override;
	void ResetVerticesAndIndices() noexcept override;

	[[nodiscard]]
	const void* GetVertexData() const noexcept override;
	[[nodiscard]]
	size_t GetVertexStrideSize() const noexcept override;
	[[nodiscard]]
	size_t GetVertexBufferSize() const noexcept override;
	[[nodiscard]]
	const void* GetIndexData() const noexcept override;
	[[nodiscard]]
	size_t GetIndexBufferSize() const noexcept override;
	[[nodiscard]]
	size_t GetIndexCount() const noexcept override;
	[[nodiscard]]
	const std::vector<VertexElementType>& GetVertexLayout() const noexcept override;

	[[nodiscard]]
	Ceres::Matrix GetTransform() const noexcept override;
	[[nodiscard]]
	Ceres::Float32_4 GetSolidColor() const noexcept override;

protected:
	std::vector<Vertex> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	Ceres::Float32_4 m_solidColor;
	Ceres::Matrix m_transform;
};
#endif
