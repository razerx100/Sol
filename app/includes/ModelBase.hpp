#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>
#include <SolConcepts.hpp>

#include <MeshBundle.hpp>
#include <Model.hpp>

class ModelInputs
{
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
	void SetInputName(const std::string& name) noexcept;

private:
	[[nodiscard]]
	DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) const noexcept;

private:
	std::string m_name;

protected:
	std::vector<Vertex> m_vertices;
	std::vector<std::uint32_t> m_indices;
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

/*class ModelBoundingBox {
public:
	ModelBoundingBox() noexcept;

	void Calculate(const std::vector<Vertex>& vertices) noexcept;
	void SetBoundingCube(const ModelBounds& bounds) noexcept;

	[[nodiscard]]
	ModelBounds GetBoundingCube() const noexcept;

private:
	ModelBounds m_boundingCube;
};*/

class ModelBase : public virtual Model
{
public:
	ModelBase() noexcept;
	virtual ~ModelBase() = default;

	void SetAsLightSource() noexcept;
	void SetMeshIndex(std::uint32_t index) noexcept { m_meshIndex = index; }

	virtual void PhysicsUpdate() noexcept;
	virtual void SetResources();

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final;
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final;
	[[nodiscard]]
	bool IsLightSource() const noexcept final;
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept final { return m_meshIndex; }
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept final { return m_materialIndex; }

	[[nodiscard]]
	ModelTransform& GetTransform() noexcept;

private:
	std::uint32_t  m_meshIndex;
	std::uint32_t  m_materialIndex;
	ModelTransform m_transform;

	bool m_lightSource;
};

class ModelBaseVS : public ModelVS
{
public:
	ModelBaseVS()
		: ModelVS{}, m_meshDetails{ .indexCount = 0u, .indexOffset = 0u }
	{}

	void SetIndexOffset(std::uint32_t offset) noexcept { m_meshDetails.indexOffset = offset; }
	void SetIndexCount(std::uint32_t count) noexcept { m_meshDetails.indexCount = count; }
	void SetMeshDetailsVS(const MeshDetailsVS& meshDetails) noexcept { m_meshDetails = meshDetails; }

	[[nodiscard]]
	const MeshDetailsVS& GetMeshDetailsVS() const noexcept override { return m_meshDetails; }

private:
	MeshDetailsVS m_meshDetails;
};

class ModelBaseMS : public ModelMS
{
public:
	ModelBaseMS()
		: ModelMS{}, m_meshDetails{ .meshlets = {} }
	{}

	void SetMeshlets(std::vector<Meshlet>&& meshlets) noexcept
	{
		m_meshDetails.meshlets = std::move(meshlets);
	}
	// I should add a function which should take a Mesh and grab its meshlets.

	[[nodiscard]]
	MeshDetailsMS& GetMeshDetailsMS() noexcept override
	{
		return m_meshDetails;
	}

private:
	MeshDetailsMS m_meshDetails;
};

// Mostly I should have different child classes of ModelBase with different functionalities.
// And then in the end before adding them to the renderer, add a wrapper based on the pipeline type.
template<class Derived>
class ModelBaseVSWrapper : public Derived, public ModelBaseVS {};
template<class Derived>
class ModelBaseMSWrapper : public Derived, public ModelBaseMS {};
#endif
