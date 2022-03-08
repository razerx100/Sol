#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <ISolModel.hpp>

struct Vertex {
	Ceres::Float32_3 position;
	Ceres::Float32_2 uv;
};

class Model : public ISolModel {
public:
	void ResetVerticesAndIndices() noexcept override;
	void SetTextureIndex(size_t index) noexcept override;

	void SetTextureName(const std::string& name) noexcept override;
	void UpdateUV(float uStart, float vStart) noexcept override;

	[[nodiscard]]
	const std::string& GetTextureName() const noexcept override;

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
	std::uint32_t GetTextureIndex() const noexcept override;
	[[nodiscard]]
	const std::vector<VertexElementType>& GetVertexLayout() const noexcept override;

	[[nodiscard]]
	Ceres::Matrix GetTransform() const noexcept override;

protected:
	std::uint32_t m_textureIndex;
	std::vector<Vertex> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	Ceres::Matrix m_transform;

	std::string m_textureName;
};
#endif
