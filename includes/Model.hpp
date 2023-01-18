#ifndef MODEL_HPP_
#define MODEL_HPP_
#include <vector>
#include <string>
#include <array>
#include <SolConcepts.hpp>

#include <IModel.hpp>

class ModelInputs {
public:
	ModelInputs() = default;
	ModelInputs(const std::string& name) noexcept;

	virtual ~ModelInputs() = default;
	virtual void InitData() noexcept;

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

	template<DerivedWithoutArgs<ModelInputs> T>
	[[nodiscard]]
	static std::string GetName() noexcept {
		T modelInput{};

		return modelInput.m_name;
	}

	template<DerivedWithArgs<ModelInputs> T>
	[[nodiscard]]
	static std::string GetName(const typename T::Args& arguments) noexcept {
		T modelInput{ arguments };

		return modelInput.m_name;
	}

protected:
	void CalculateNormalsIndependentFaces() noexcept;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) const noexcept;

protected:
	std::vector<Vertex> m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::string m_name;
};

class ModelTransform {
public:
	ModelTransform() noexcept;

	ModelTransform& RotatePitchDegree(float angle) noexcept;
	ModelTransform& RotateYawDegree(float angle) noexcept;
	ModelTransform& RotateRollDegree(float angle) noexcept;
	ModelTransform& RotatePitchRadian(float angle) noexcept;
	ModelTransform& RotateYawRadian(float angle) noexcept;
	ModelTransform& RotateRollRadian(float angle) noexcept;

	ModelTransform& MoveTowardsX(float delta) noexcept;
	ModelTransform& MoveTowardsY(float delta) noexcept;
	ModelTransform& MoveTowardsZ(float delta) noexcept;

	void Rotate(const DirectX::XMVECTOR& rotationAxis, float angleRadian) noexcept;
	void MultiplyModelMatrix(const DirectX::XMMATRIX& matrix) noexcept;
	void AddToModelOffset(const DirectX::XMFLOAT3& offset) noexcept;
	void SetModelOffset(const DirectX::XMFLOAT3& offset) noexcept;

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept;
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept;

private:
	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMFLOAT3 m_modelOffset;
};

class ModelBoundingBox {
public:
	ModelBoundingBox() noexcept;

	void Calculate(const std::vector<Vertex>& vertices) noexcept;
	void SetBoundingCube(const ModelBounds& bounds) noexcept;

	[[nodiscard]]
	ModelBounds GetBoundingCube() const noexcept;

private:
	ModelBounds m_boundingCube;
};

class Model : public IModel {
public:
	Model() noexcept;
	virtual ~Model() = default;

	void SetTextureIndex(size_t index) noexcept;
	void SetUVInfo(float uOffset, float vOffset, float uRatio, float vRatio) noexcept;
	void SetUVInfo(const UVInfo& uvInfo) noexcept;
	void SetIndexOffset(std::uint32_t indexOffset) noexcept;
	void SetIndexCount(std::uint32_t indexCount) noexcept;
	void SetBoundingBox(const ModelBoundingBox& boundingBox) noexcept;
	void SetAsLightSource() noexcept;
	void SetMaterial(const Material& material) noexcept;

	virtual void PhysicsUpdate() noexcept;
	virtual void SetResources();

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
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final;
	[[nodiscard]]
	ModelBounds GetBoundingBox() const noexcept final;
	[[nodiscard]]
	Material GetMaterial() const noexcept final;
	[[nodiscard]]
	bool IsLightSource() const noexcept final;

	[[nodiscard]]
	ModelTransform& GetTransform() noexcept;
	[[nodiscard]]
	ModelBoundingBox& GetBoundingBox() noexcept;

private:
	std::uint32_t m_textureIndex;

	UVInfo m_uvInfo;
	std::uint32_t m_indexCount;
	std::uint32_t m_indexOffset;
	ModelTransform m_transform;
	ModelBoundingBox m_boundingBox;

	Material m_material;
	bool m_lightSource;
};
#endif
