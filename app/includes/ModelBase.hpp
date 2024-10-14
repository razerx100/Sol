#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>

#include <MeshBundleBase.hpp>
#include <Model.hpp>
#include <Renderer.hpp>

class ModelTransform
{
public:
	ModelTransform()
		: m_modelMatrix{ DirectX::XMMatrixIdentity() }, m_modelOffset{ 0.f, 0.f, 0.f }
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
		static const DirectX::XMVECTOR pitchAxis{ DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f) };
		Rotate(pitchAxis, angle);

		return *this;
	}
	ModelTransform& RotateYawRadian(float angle) noexcept
	{
		static const DirectX::XMVECTOR yawAxis{ DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f) };
		Rotate(yawAxis, angle);

		return *this;
	}
	ModelTransform& RotateRollRadian(float angle) noexcept
	{
		static const DirectX::XMVECTOR rollAxis{ DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f) };
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
	void MultiplyModelMatrix(const DirectX::XMMATRIX& matrix) noexcept { m_modelMatrix *= matrix; }
	void AddToModelOffset(const DirectX::XMFLOAT3& offset) noexcept
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

private:
	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMFLOAT3 m_modelOffset;
};

class ModelBase
{
public:
	ModelBase()
		: m_materialIndex{ 0u }, m_diffuseIndex{ 0u }, m_specularIndex{ 0u },
		m_transform{}, m_diffuseUVInfo{ 0.f, 0.f, 1.f, 1.f }, m_specularUVInfo{ 0.f, 0.f, 1.f, 1.f }
	{}
	ModelBase(float scale) : ModelBase{}
	{
		GetTransform().MultiplyModelMatrix(DirectX::XMMatrixScaling(scale, scale, scale));
	}

	void SetMaterialIndex(std::uint32_t index) noexcept { m_materialIndex = index; }

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept { return m_transform.GetModelMatrix(); }
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept { return m_transform.GetModelOffset(); }
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept { return m_materialIndex; }

	[[nodiscard]]
	ModelTransform& GetTransform() noexcept { return m_transform; }
	[[nodiscard]]
	const ModelTransform& GetTransform() const noexcept { return m_transform; }

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

	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept { return m_diffuseIndex; }
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept { return m_diffuseUVInfo; }
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept { return m_specularIndex; }
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept { return m_specularUVInfo; }

private:
	std::uint32_t  m_materialIndex;
	std::uint32_t  m_diffuseIndex;
	std::uint32_t  m_specularIndex;
	ModelTransform m_transform;
	UVInfo         m_diffuseUVInfo;
	UVInfo         m_specularUVInfo;
};

class ModelBaseVS : public ModelVS
{
public:
	ModelBaseVS(const ModelBase& modelBase)
		: ModelVS{}, m_meshDetails{ .indexCount = 0u, .indexOffset = 0u }, m_modelBase{ modelBase }
	{}

	void SetIndexOffset(std::uint32_t offset) noexcept { m_meshDetails.indexOffset = offset; }
	void SetIndexCount(std::uint32_t count) noexcept { m_meshDetails.indexCount = count; }
	void SetMeshDetailsVS(MeshDetailsVS meshDetails) noexcept { m_meshDetails = meshDetails; }

	[[nodiscard]]
	MeshDetailsVS GetMeshDetailsVS() const noexcept override { return m_meshDetails; }

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final
	{
		return m_modelBase.GetModelMatrix();
	}
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final
	{
		return m_modelBase.GetModelOffset();
	}
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept final
	{
		return m_modelBase.GetMaterialIndex();
	}
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override
	{
		return m_modelBase.GetDiffuseIndex();
	}
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override
	{
		return m_modelBase.GetDiffuseUVInfo();
	}
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override
	{
		return m_modelBase.GetSpecularIndex();
	}
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override
	{
		return m_modelBase.GetSpecularUVInfo();
	}

	ModelBase& GetBase() noexcept { return m_modelBase; }

private:
	MeshDetailsVS m_meshDetails;
	ModelBase     m_modelBase;

public:
	ModelBaseVS(const ModelBaseVS& other) noexcept
		: m_meshDetails{ other.m_meshDetails }, m_modelBase{ other.m_modelBase }
	{}
	ModelBaseVS& operator=(const ModelBaseVS& other) noexcept
	{
		m_meshDetails = other.m_meshDetails;
		m_modelBase   = other.m_modelBase;

		return *this;
	}

	ModelBaseVS(ModelBaseVS&& other) noexcept
		: m_meshDetails{ std::move(other.m_meshDetails) }, m_modelBase{ std::move(other.m_modelBase) }
	{}
	ModelBaseVS& operator=(ModelBaseVS&& other) noexcept
	{
		m_meshDetails = std::move(other.m_meshDetails);
		m_modelBase   = std::move(other.m_modelBase);

		return *this;
	}
};

class ModelBaseMS : public ModelMS
{
public:
	ModelBaseMS(const ModelBase& modelBase)
		: ModelMS{}, m_meshDetails{}, m_modelBase{ modelBase }
	{}

	void SetMeshletOffset(std::uint32_t offset) noexcept { m_meshDetails.meshletOffset = offset; }
	void SetMeshletCount(std::uint32_t count) noexcept { m_meshDetails.meshletCount = count; }
	void SetMeshDetailsMS(MeshDetailsMS meshDetails) noexcept { m_meshDetails = meshDetails; }

	[[nodiscard]]
	MeshDetailsMS GetMeshDetailsMS() const noexcept override
	{
		return m_meshDetails;
	}

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final
	{
		return m_modelBase.GetModelMatrix();
	}
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final
	{
		return m_modelBase.GetModelOffset();
	}
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept final
	{
		return m_modelBase.GetMaterialIndex();
	}
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override
	{
		return m_modelBase.GetDiffuseIndex();
	}
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override
	{
		return m_modelBase.GetDiffuseUVInfo();
	}
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override
	{
		return m_modelBase.GetSpecularIndex();
	}
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override
	{
		return m_modelBase.GetSpecularUVInfo();
	}

	ModelBase& GetBase() noexcept { return m_modelBase; }

private:
	MeshDetailsMS m_meshDetails;
	ModelBase     m_modelBase;

public:
	ModelBaseMS(const ModelBaseMS& other) noexcept
		: m_meshDetails{ other.m_meshDetails }, m_modelBase{ other.m_modelBase }
	{}
	ModelBaseMS& operator=(const ModelBaseMS& other) noexcept
	{
		m_meshDetails = other.m_meshDetails;
		m_modelBase   = other.m_modelBase;

		return *this;
	}

	ModelBaseMS(ModelBaseMS&& other) noexcept
		: m_meshDetails{ std::move(other.m_meshDetails) }, m_modelBase{ std::move(other.m_modelBase) }
	{}
	ModelBaseMS& operator=(ModelBaseMS&& other) noexcept
	{
		m_meshDetails = std::move(other.m_meshDetails);
		m_modelBase   = std::move(other.m_modelBase);

		return *this;
	}
};

class ModelBundleImplVS : public ModelBundleVS
{
	friend class ModelBundleBaseVS;

	ModelBundleImplVS() : ModelBundleVS{}, m_models{}, m_meshIndex{} {}
public:

	[[nodiscard]]
	const std::vector<std::shared_ptr<ModelVS>>& GetModels() const noexcept override { return m_models; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept override { return *m_meshIndex; }

private:
	std::vector<std::shared_ptr<ModelVS>> m_models;
	std::shared_ptr<std::uint32_t>        m_meshIndex;

public:
	ModelBundleImplVS(const ModelBundleImplVS& other) noexcept
		: m_models{ other.m_models }, m_meshIndex{ other.m_meshIndex }
	{}
	ModelBundleImplVS& operator=(const ModelBundleImplVS& other) noexcept
	{
		m_models    = other.m_models;
		m_meshIndex = other.m_meshIndex;

		return *this;
	}
	ModelBundleImplVS(ModelBundleImplVS&& other) noexcept
		: m_models{ std::move(other.m_models) }, m_meshIndex{ std::move(other.m_meshIndex) }
	{}
	ModelBundleImplVS& operator=(ModelBundleImplVS&& other) noexcept
	{
		m_models    = std::move(other.m_models);
		m_meshIndex = std::move(other.m_meshIndex);

		return *this;
	}
};

class ModelBundleImplMS : public ModelBundleMS
{
	friend class ModelBundleBaseMS;

	ModelBundleImplMS() : ModelBundleMS{}, m_models{}, m_meshIndex{} {}
public:

	[[nodiscard]]
	const std::vector<std::shared_ptr<ModelMS>>& GetModels() const noexcept override { return m_models; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept override { return *m_meshIndex; }

private:
	std::vector<std::shared_ptr<ModelMS>> m_models;
	std::shared_ptr<std::uint32_t>        m_meshIndex;

public:
	ModelBundleImplMS(const ModelBundleImplMS& other) noexcept
		: m_models{ other.m_models }, m_meshIndex{ other.m_meshIndex }
	{}
	ModelBundleImplMS& operator=(const ModelBundleImplMS& other) noexcept
	{
		m_models    = other.m_models;
		m_meshIndex = other.m_meshIndex;

		return *this;
	}
	ModelBundleImplMS(ModelBundleImplMS&& other) noexcept
		: m_models{ std::move(other.m_models) }, m_meshIndex{ std::move(other.m_meshIndex) }
	{}
	ModelBundleImplMS& operator=(ModelBundleImplMS&& other) noexcept
	{
		m_models    = std::move(other.m_models);
		m_meshIndex = std::move(other.m_meshIndex);

		return *this;
	}
};

class ModelBundleBaseVS
{
public:
	ModelBundleBaseVS() : m_models{}, m_meshIndex{ std::make_shared<std::uint32_t>(0u) } {}

	void AddModel(std::shared_ptr<ModelBaseVS> model) noexcept
	{
		m_models.emplace_back(std::move(model));
	}
	void SetMeshIndex(std::uint32_t index) noexcept
	{
		*m_meshIndex = index;
	}

	[[nodiscard]]
	std::vector<std::shared_ptr<ModelBaseVS>>& GetModels() noexcept { return m_models; }
	[[nodiscard]]
	std::shared_ptr<ModelBaseVS>& GetModel(size_t index) noexcept { return m_models[index]; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept { return *m_meshIndex; }

	[[nodiscard]]
	std::shared_ptr<ModelBundleImplVS> GetBundleImpl() const noexcept;

private:
	std::vector<std::shared_ptr<ModelBaseVS>> m_models;
	std::shared_ptr<std::uint32_t>            m_meshIndex;

public:
	ModelBundleBaseVS(const ModelBundleBaseVS& other) noexcept
		: m_models{ other.m_models }, m_meshIndex{ other.m_meshIndex }
	{}
	ModelBundleBaseVS& operator=(const ModelBundleBaseVS& other) noexcept
	{
		m_models    = other.m_models;
		m_meshIndex = other.m_meshIndex;

		return *this;
	}
	ModelBundleBaseVS(ModelBundleBaseVS&& other) noexcept
		: m_models{ std::move(other.m_models) }, m_meshIndex{ std::move(other.m_meshIndex) }
	{}
	ModelBundleBaseVS& operator=(ModelBundleBaseVS&& other) noexcept
	{
		m_models    = std::move(other.m_models);
		m_meshIndex = std::move(other.m_meshIndex);

		return *this;
	}
};

class ModelBundleBaseMS
{
public:
	ModelBundleBaseMS() : m_models{}, m_meshIndex{ std::make_shared<std::uint32_t>(0u) } {}

	void AddModel(std::shared_ptr<ModelBaseMS> model) noexcept
	{
		m_models.emplace_back(std::move(model));
	}
	void SetMeshIndex(std::uint32_t index) noexcept
	{
		*m_meshIndex = index;
	}

	[[nodiscard]]
	std::vector<std::shared_ptr<ModelBaseMS>>& GetModels() noexcept { return m_models; }
	[[nodiscard]]
	std::shared_ptr<ModelBaseMS>& GetModel(size_t index) noexcept { return m_models[index]; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept { return *m_meshIndex; }

	[[nodiscard]]
	std::shared_ptr<ModelBundleImplMS> GetBundleImpl() const noexcept;

private:
	std::vector<std::shared_ptr<ModelBaseMS>> m_models;
	std::shared_ptr<std::uint32_t>            m_meshIndex;

public:
	ModelBundleBaseMS(const ModelBundleBaseMS& other) noexcept
		: m_models{ other.m_models }, m_meshIndex{ other.m_meshIndex }
	{}
	ModelBundleBaseMS& operator=(const ModelBundleBaseMS& other) noexcept
	{
		m_models    = other.m_models;
		m_meshIndex = other.m_meshIndex;

		return *this;
	}
	ModelBundleBaseMS(ModelBundleBaseMS&& other) noexcept
		: m_models{ std::move(other.m_models) }, m_meshIndex{ std::move(other.m_meshIndex) }
	{}
	ModelBundleBaseMS& operator=(ModelBundleBaseMS&& other) noexcept
	{
		m_models    = std::move(other.m_models);
		m_meshIndex = std::move(other.m_meshIndex);

		return *this;
	}
};

class ModelBundle
{
public:
	ModelBundle()
		: m_bundleVS{ s_modelTypeVS ? std::make_unique<ModelBundleBaseVS>() : nullptr },
		m_bundleMS{ s_modelTypeVS ? nullptr : std::make_unique<ModelBundleBaseMS>() }
	{}

	static void SetModelType(bool meshShader) noexcept { s_modelTypeVS = !meshShader; }

	ModelBundle& AddModel(float scale = 1.f) noexcept;

	void SetMeshModelDetails(size_t modelIndex, const MeshDetails& meshDetails) noexcept;
	void SetMeshIndex(std::uint32_t index) noexcept;

	[[nodiscard]]
	ModelBase& GetModel(size_t index) noexcept;
	[[nodiscard]]
	const ModelBase& GetModel(size_t index) const noexcept { return GetModel(index); }

	[[nodiscard]]
	std::uint32_t SetModelBundle(Renderer& renderer, const ShaderName& pixelShaderName);

private:
	std::unique_ptr<ModelBundleBaseVS> m_bundleVS;
	std::unique_ptr<ModelBundleBaseMS> m_bundleMS;

	inline static bool s_modelTypeVS = true;

public:
	ModelBundle(const ModelBundle&) = delete;
	ModelBundle& operator=(const ModelBundle&) = delete;

	ModelBundle(ModelBundle&& other) noexcept
		: m_bundleVS{ std::move(other.m_bundleVS) },
		m_bundleMS{ std::move(other.m_bundleMS) }
	{}
	ModelBundle& operator=(ModelBundle&& other) noexcept
	{
		m_bundleVS = std::move(other.m_bundleVS);
		m_bundleMS = std::move(other.m_bundleMS);

		return *this;
	}
};
#endif
