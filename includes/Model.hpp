#ifndef MODEL_HPP_
#define MODEL_HPP_
#include <vector>
#include <cstring>

#include <IModel.hpp>

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};

class ModelInputs {
public:
	[[nodiscard]]
	const std::vector<Vertex>& GetVertexData() const noexcept;
	[[nodiscard]]
	size_t GetVertexBufferSize() const noexcept;
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndexData() const noexcept;
	[[nodiscard]]
	size_t GetIndexBufferSize() const noexcept;

	[[nodiscard]]
	std::uint32_t GetIndexCount() const noexcept;

protected:
	std::vector<Vertex> m_vertices;
	std::vector<std::uint32_t> m_indices;
};

class Model : public IModel {
public:
	Model(std::uint32_t indexCount) noexcept;

	void SetTextureIndex(size_t index) noexcept;
	void SetUVInfo(float uOffset, float vOffset, float uRatio, float vRatio) noexcept;
	void SetUVInfo(const UVInfo& uvInfo) noexcept;
	void SetIndexOffset(std::uint32_t indexOffset) noexcept;

	[[nodiscard]]
	std::uint32_t GetIndexCount() const noexcept final;
	[[nodiscard]]
	std::uint32_t GetIndexOffset() const noexcept final;
	[[nodiscard]]
	std::uint32_t GetTextureIndex() const noexcept final;
	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final;
	[[nodiscard]]
	UVInfo GetUVInfo() const noexcept final;

	void SetModelMatrix(const DirectX::XMMATRIX& modelMatrix) noexcept;
	void AddTransformation(const DirectX::XMMATRIX& transform) noexcept;

protected:
	DirectX::XMMATRIX m_modelMatrix;

private:
	std::uint32_t m_textureIndex;

	UVInfo m_uvInfo;
	std::uint32_t m_indexCount;
	std::uint32_t m_indexOffset;
};
#endif
