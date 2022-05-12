#ifndef MODEL_HPP_
#define MODEL_HPP_
#include <vector>
#include <IModel.hpp>
#include <DirectXMath.h>

struct Vertex {
	DirectX::XMFLOAT3 position;
	//DirectX::XMFLOAT2 uv;
};

class Model : public IModel {
public:
	Model() noexcept;

	void ResetVerticesAndIndices() noexcept;
	void SetTextureIndex(size_t index) noexcept;

	void SetTextureInfo(
		const TextureData& textureInfo
	) noexcept;

	[[nodiscard]]
	const void* GetVertexData() const noexcept final;
	[[nodiscard]]
	size_t GetVertexStrideSize() const noexcept final;
	[[nodiscard]]
	size_t GetVertexBufferSize() const noexcept final;
	[[nodiscard]]
	const void* GetIndexData() const noexcept final;
	[[nodiscard]]
	size_t GetIndexBufferSize() const noexcept final;
	[[nodiscard]]
	size_t GetIndexCount() const noexcept final;
	[[nodiscard]]
	std::uint32_t GetTextureIndex() const noexcept final;
	[[nodiscard]]
	const std::vector<VertexElementType>& GetVertexLayout() const noexcept final;

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final;

	const TextureData& GetTextureInfo() const noexcept final;

	void SetModelMatrix(const DirectX::XMMATRIX& modelMatrix) noexcept;

protected:
	std::uint32_t m_textureIndex;
	std::vector<Vertex> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	DirectX::XMMATRIX m_modelMatrix;

	TextureData m_textureData;
};
#endif
