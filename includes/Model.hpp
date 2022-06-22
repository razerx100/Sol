#ifndef MODEL_HPP_
#define MODEL_HPP_
#include <vector>
#include <cstring>

#include <IModel.hpp>

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
	std::unique_ptr<std::uint8_t> GetVertexData() const noexcept final;
	[[nodiscard]]
	size_t GetVertexStrideSize() const noexcept final;
	[[nodiscard]]
	size_t GetVertexBufferSize() const noexcept final;
	[[nodiscard]]
	std::unique_ptr<std::uint8_t> GetIndexData() const noexcept final;
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
	void AddTransformation(const DirectX::XMMATRIX& transform) noexcept;

protected:
	std::uint32_t m_textureIndex;
	std::vector<Vertex> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<VertexElementType> m_vertexLayout;
	DirectX::XMMATRIX m_modelMatrix;

	TextureData m_textureData;

private:
	template<typename T>
	static std::unique_ptr<std::uint8_t> GetDataFromVector(const std::vector<T>& vec) noexcept {
		size_t dataSize = std::size(vec) * sizeof(T);

		auto data = std::unique_ptr<std::uint8_t>(new std::uint8_t[dataSize]);

		memcpy(data.get(), std::data(vec), dataSize);

		return data;
	}
};
#endif
