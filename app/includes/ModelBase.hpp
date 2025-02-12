#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>

#include <SolMeshUtility.hpp>
#include <Model.hpp>
#include <Renderer.hpp>

class ModelTransform
{
public:
	ModelTransform()
		: m_modelMatrix{ DirectX::XMMatrixIdentity() }, m_modelOffset{ 0.f, 0.f, 0.f },
		m_modelScale{ 1.f }
	{}

	ModelTransform& RotatePitchDegree(float angle) noexcept
	{
		return RotatePitchRadian(DirectX::XMConvertToRadians(angle));
	}
	ModelTransform& RotateYawDegree(float angle) noexcept
	{
		return RotateYawRadian(DirectX::XMConvertToRadians(angle));
	}
	ModelTransform& RotateRollDegree(float angle) noexcept
	{
		return RotateRollRadian(DirectX::XMConvertToRadians(angle));
	}
	ModelTransform& RotatePitchRadian(float angle) noexcept
	{
		const DirectX::XMVECTOR pitchAxis{ DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f) };
		Rotate(pitchAxis, angle);

		return *this;
	}
	ModelTransform& RotateYawRadian(float angle) noexcept
	{
		const DirectX::XMVECTOR yawAxis{ DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f) };
		Rotate(yawAxis, angle);

		return *this;
	}
	ModelTransform& RotateRollRadian(float angle) noexcept
	{
		const DirectX::XMVECTOR rollAxis{ DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		Rotate(rollAxis, angle);

		return *this;
	}

	ModelTransform& MoveTowardsX(float delta) noexcept
	{
		m_modelOffset.x += delta;

		return *this;
	}
	ModelTransform& MoveTowardsY(float delta) noexcept
	{
		m_modelOffset.y += delta;

		return *this;
	}
	ModelTransform& MoveTowardsZ(float delta) noexcept
	{
		m_modelOffset.z += delta;

		return *this;
	}

	void Rotate(const DirectX::XMVECTOR& rotationAxis, float angleRadian) noexcept
	{
		m_modelMatrix *= DirectX::XMMatrixRotationAxis(rotationAxis, angleRadian);
	}
	void Scale(float scale) noexcept
	{
		m_modelMatrix *= DirectX::XMMatrixScaling(scale, scale, scale);

		RecalculateScale();
	}
	void MultiplyModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
	{
		m_modelMatrix *= matrix;

		RecalculateScale();
	}
	void SetModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
	{
		m_modelMatrix = matrix;
	}
	void MultiplyAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept;
	void SetAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept;
	void ResetTransform() noexcept
	{
		m_modelMatrix = DirectX::XMMatrixIdentity();
		m_modelOffset = DirectX::XMFLOAT3{ 0.f, 0.f, 0.f };
		m_modelScale  = 1.f;
	}
	void MoveModel(const DirectX::XMFLOAT3& offset) noexcept
	{
		m_modelOffset.x += offset.x;
		m_modelOffset.y += offset.y;
		m_modelOffset.z += offset.z;
	}
	void SetModelOffset(const DirectX::XMFLOAT3& offset) noexcept { m_modelOffset = offset; }

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept { return m_modelMatrix; }
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept { return m_modelOffset; }
	[[nodiscard]]
	float GetModelScale() const noexcept { return m_modelScale; }

private:
	struct BrokenDownMatrix
	{
		DirectX::XMMATRIX matrix;
		DirectX::XMFLOAT3 position;
		float             scale;
	};

private:
	void RecalculateScale() noexcept;

	[[nodiscard]]
	static BrokenDownMatrix BreakDownMatrix(const DirectX::XMMATRIX& matrix) noexcept;

private:
	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMFLOAT3 m_modelOffset;
	float             m_modelScale;
};

class ModelBase : public Model
{
	friend class ModelBundleBase;

public:
	ModelBase()
		: m_materialIndex{ 0u }, m_diffuseIndex{ 0u }, m_specularIndex{ 0u }, m_meshIndex{ 0u },
		m_pipelineIndex{ 0u }, m_visible{ true }, m_transform{}, m_diffuseUVInfo{0.f, 0.f, 1.f, 1.f},
		m_specularUVInfo{ 0.f, 0.f, 1.f, 1.f }
	{}
	ModelBase(float scale) : ModelBase{}
	{
		Scale(scale);
	}

	void SetMaterialIndex(std::uint32_t index) noexcept { m_materialIndex = index; }
	void SetMeshIndex(std::uint32_t index) noexcept { m_meshIndex = index; }

	void Scale(float scale) noexcept
	{
		GetTransform().Scale(scale);
	}

	void SetDiffuseIndex(size_t index) noexcept
	{
		m_diffuseIndex = static_cast<std::uint32_t>(index);
	}
	void SetSpecularIndex(size_t index) noexcept
	{
		m_specularIndex = static_cast<std::uint32_t>(index);
	}

	void SetDiffuseUVInfo(float uOffset, float vOffset, float uScale, float vScale) noexcept
	{
		SetDiffuseUVInfo(
			UVInfo{ .uOffset = uOffset, .vOffset = vOffset, .uScale = uScale, .vScale = vScale }
		);
	}
	void SetDiffuseUVInfo(const UVInfo& uvInfo) noexcept { m_diffuseUVInfo = uvInfo; }
	void SetSpecularUVInfo(float uOffset, float vOffset, float uScale, float vScale) noexcept
	{
		SetSpecularUVInfo(
			UVInfo{ .uOffset = uOffset, .vOffset = vOffset, .uScale = uScale, .vScale = vScale }
		);
	}
	void SetSpecularUVInfo(const UVInfo& uvInfo) noexcept { m_specularUVInfo = uvInfo; }
	void SetPipelineIndex(std::uint32_t pipelineIndex) noexcept { m_pipelineIndex = pipelineIndex; }
	void SetVisibility(bool value) noexcept { m_visible = value; }

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept override { return m_transform.GetModelMatrix(); }
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept override { return m_transform.GetModelOffset(); }
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept override { return m_materialIndex; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept override { return m_meshIndex; }
	[[nodiscard]]
	float GetModelScale() const noexcept override { return m_transform.GetModelScale(); }
	[[nodiscard]]
	std::uint32_t GetPipelineIndex() const noexcept override { return m_pipelineIndex; }
	[[nodiscard]]
	bool IsVisible() const noexcept override { return m_visible; }

	[[nodiscard]]
	const ModelTransform& GetTransform() const noexcept { return m_transform; }
	[[nodiscard]]
	ModelTransform& GetTransform() noexcept { return m_transform; }

	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override { return m_diffuseIndex; }
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override { return m_diffuseUVInfo; }
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override { return m_specularIndex; }
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override { return m_specularUVInfo; }

private:
	std::uint32_t  m_materialIndex;
	std::uint32_t  m_diffuseIndex;
	std::uint32_t  m_specularIndex;
	std::uint32_t  m_meshIndex;
	std::uint32_t  m_pipelineIndex;
	bool           m_visible;
	ModelTransform m_transform;
	UVInfo         m_diffuseUVInfo;
	UVInfo         m_specularUVInfo;
};

class ModelBundleImpl : public ModelBundle
{
	friend class ModelBundleBase;
public:
	ModelBundleImpl() : ModelBundle{}, m_models{}, m_meshBundleIndex{} {}

	[[nodiscard]]
	const std::vector<std::shared_ptr<Model>>& GetModels() const noexcept override { return m_models; }
	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept override { return *m_meshBundleIndex; }

private:
	std::vector<std::shared_ptr<Model>> m_models;
	std::shared_ptr<std::uint32_t>      m_meshBundleIndex;

public:
	ModelBundleImpl(const ModelBundleImpl& other) noexcept
		: m_models{ other.m_models }, m_meshBundleIndex{ other.m_meshBundleIndex }
	{}
	ModelBundleImpl& operator=(const ModelBundleImpl& other) noexcept
	{
		m_models          = other.m_models;
		m_meshBundleIndex = other.m_meshBundleIndex;

		return *this;
	}
	ModelBundleImpl(ModelBundleImpl&& other) noexcept
		: m_models{ std::move(other.m_models) },
		m_meshBundleIndex{ std::move(other.m_meshBundleIndex) }
	{}
	ModelBundleImpl& operator=(ModelBundleImpl&& other) noexcept
	{
		m_models          = std::move(other.m_models);
		m_meshBundleIndex = std::move(other.m_meshBundleIndex);

		return *this;
	}
};

class ModelBundleBase
{
public:
	ModelBundleBase()
		: m_modelNodeData{}, m_models{}, m_meshBundleIndex{ std::make_shared<std::uint32_t>(0u) }
	{}

	ModelBundleBase& AddModel(float scale) noexcept;

	void SetMeshBundleIndex(std::uint32_t index) noexcept
	{
		*m_meshBundleIndex = index;
	}

	// Because of circular inclusion, can't include the MeshBundleBase header in this header.
	// So, have to do some template stuff to figure that out.
	template<class T>
	void SetMeshBundle(
		std::uint32_t meshBundleIndex, float modelScale, const T& meshBundle
	) {
		const std::vector<MeshNodeData>& newNodeData              = meshBundle.GetMeshNodeData();
		const std::vector<MeshPermanentDetails>& permanentDetails = meshBundle.GetPermanentDetails();

		SetMeshBundle(meshBundleIndex, modelScale, newNodeData, permanentDetails);
	}

	void SetMeshBundle(
		std::uint32_t meshBundleIndex, float modelScale,
		const std::vector<MeshNodeData>& newNodeData,
		const std::vector<MeshPermanentDetails>& permanentDetails
	);

	// Because of circular inclusion, can't include the MeshBundleBase header in this header.
	// So, have to do some template stuff to figure that out.
	template<class T>
	void ChangeMeshBundle(
		std::uint32_t meshBundleIndex, const T& meshBundle, bool discardExistingTransformation = true
	) {
		const std::vector<MeshNodeData>& newNodeData              = meshBundle.GetMeshNodeData();
		const std::vector<MeshPermanentDetails>& permanentDetails = meshBundle.GetPermanentDetails();

		ChangeMeshBundle(meshBundleIndex, newNodeData, permanentDetails, discardExistingTransformation);
	}

	// Can only be changed if the new mesh count is the same as before.
	void ChangeMeshBundle(
		std::uint32_t meshBundleIndex,
		const std::vector<MeshNodeData>& newNodeData,
		const std::vector<MeshPermanentDetails>& permanentDetails,
		bool discardExistingTransformation = true
	);

	// Transform
	ModelBundleBase& MoveTowardsX(size_t nodeIndex, float delta) noexcept
	{
		MoveModel(nodeIndex, DirectX::XMFLOAT3{ delta, 0.f, 0.f });

		return *this;
	}
	ModelBundleBase& MoveTowardsY(size_t nodeIndex, float delta) noexcept
	{
		MoveModel(nodeIndex, DirectX::XMFLOAT3{ 0.f, delta, 0.f });

		return *this;
	}
	ModelBundleBase& MoveTowardsZ(size_t nodeIndex, float delta) noexcept
	{
		MoveModel(nodeIndex, DirectX::XMFLOAT3{ 0.f, 0.f, delta });

		return *this;
	}

	ModelBundleBase& RotatePitchDegree(size_t nodeIndex, float angle) noexcept
	{
		return RotatePitchRadian(nodeIndex, DirectX::XMConvertToRadians(angle));
	}
	ModelBundleBase& RotateYawDegree(size_t nodeIndex, float angle) noexcept
	{
		return RotateYawRadian(nodeIndex, DirectX::XMConvertToRadians(angle));
	}
	ModelBundleBase& RotateRollDegree(size_t nodeIndex, float angle) noexcept
	{
		return RotateRollRadian(nodeIndex, DirectX::XMConvertToRadians(angle));
	}
	ModelBundleBase& RotatePitchRadian(size_t nodeIndex, float angle) noexcept
	{
		const DirectX::XMVECTOR pitchAxis{ DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f) };
		Rotate(nodeIndex, pitchAxis, angle);

		return *this;
	}
	ModelBundleBase& RotateYawRadian(size_t nodeIndex, float angle) noexcept
	{
		const DirectX::XMVECTOR yawAxis{ DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f) };
		Rotate(nodeIndex, yawAxis, angle);

		return *this;
	}
	ModelBundleBase& RotateRollRadian(size_t nodeIndex, float angle) noexcept
	{
		const DirectX::XMVECTOR rollAxis{ DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		Rotate(nodeIndex, rollAxis, angle);

		return *this;
	}

	void Scale(size_t nodeIndex, float scale) noexcept;
	void Rotate(
		size_t nodeIndex, const DirectX::XMVECTOR& rotationAxis, float angleRadian
	) noexcept;
	void MoveModel(size_t nodeIndex, const DirectX::XMFLOAT3& offset) noexcept;

	[[nodiscard]]
	std::vector<std::shared_ptr<ModelBase>>& GetModels() noexcept { return m_models; }
	[[nodiscard]]
	const std::vector<std::shared_ptr<ModelBase>>& GetModels() const noexcept { return m_models; }

	[[nodiscard]]
	std::shared_ptr<ModelBase>& GetModel(size_t index) noexcept { return m_models[index]; }
	[[nodiscard]]
	const std::shared_ptr<ModelBase>& GetModel(size_t index) const noexcept { return m_models[index]; }
	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept { return *m_meshBundleIndex; }

	[[nodiscard]]
	std::uint32_t SetModelBundle(Renderer& renderer, const ShaderName& pixelShaderName) const;

private:
	void SetModels(float modelScale, const std::vector<MeshNodeData>& nodeData);

private:
	[[nodiscard]]
	std::shared_ptr<ModelBundleImpl> GetBundleImpl() const noexcept;

private:
	std::vector<MeshNodeData>               m_modelNodeData;
	std::vector<std::shared_ptr<ModelBase>> m_models;
	std::shared_ptr<std::uint32_t>          m_meshBundleIndex;

public:
	ModelBundleBase(const ModelBundleBase& other) noexcept
		: m_modelNodeData{ other.m_modelNodeData }, m_models { other.m_models },
		m_meshBundleIndex{ other.m_meshBundleIndex }
	{}
	ModelBundleBase& operator=(const ModelBundleBase& other) noexcept
	{
		m_modelNodeData   = other.m_modelNodeData;
		m_models          = other.m_models;
		m_meshBundleIndex = other.m_meshBundleIndex;

		return *this;
	}
	ModelBundleBase(ModelBundleBase&& other) noexcept
		: m_modelNodeData{ std::move(other.m_modelNodeData) }, m_models{ std::move(other.m_models) },
		m_meshBundleIndex{ std::move(other.m_meshBundleIndex) }
	{}
	ModelBundleBase& operator=(ModelBundleBase&& other) noexcept
	{
		m_modelNodeData   = std::move(other.m_modelNodeData);
		m_models          = std::move(other.m_models);
		m_meshBundleIndex = std::move(other.m_meshBundleIndex);

		return *this;
	}
};
#endif
