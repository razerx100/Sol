#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>
#include <SolConcepts.hpp>

#include <Model.hpp>

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
	virtual ~ModelBase() = default;

	void SetMaterialIndex(std::uint32_t index) noexcept { m_materialIndex = index; }

	virtual void PhysicsUpdate() noexcept {}
	virtual void SetResources() {}

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
	ModelBaseMS() : ModelMS{}, m_meshDetails{} {}

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

public:
	ModelBaseMS(const ModelBaseMS& other) noexcept
		: m_meshDetails{ other.m_meshDetails }
	{}
	ModelBaseMS& operator=(const ModelBaseMS& other) noexcept
	{
		m_meshDetails = other.m_meshDetails;

		return *this;
	}

	ModelBaseMS(ModelBaseMS&& other) noexcept
		: m_meshDetails{ std::move(other.m_meshDetails) }
	{}
	ModelBaseMS& operator=(ModelBaseMS&& other) noexcept
	{
		m_meshDetails = std::move(other.m_meshDetails);

		return *this;
	}
};

class ModelBundleBaseVS : public ModelBundleVS
{
public:
	ModelBundleBaseVS() : ModelBundleVS{}, m_models{}, m_meshIndex{} {}

	void AddModel(std::shared_ptr<ModelVS> model) noexcept
	{
		m_models.emplace_back(std::move(model));
	}
	void SetMeshIndex(std::uint32_t index) noexcept
	{
		m_meshIndex = index;
	}

	[[nodiscard]]
	std::shared_ptr<ModelVS>& GetModel(size_t index) noexcept { return m_models[index]; }
	[[nodiscard]]
	const std::vector<std::shared_ptr<ModelVS>>& GetModels() const noexcept override { return m_models; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept { return m_meshIndex; }

private:
	std::vector<std::shared_ptr<ModelVS>> m_models;
	std::uint32_t                         m_meshIndex;
};

class ModelBundleBaseMS : public ModelBundleMS
{
public:
	ModelBundleBaseMS() : ModelBundleMS{}, m_models{}, m_meshIndex{} {}

	void AddModel(std::shared_ptr<ModelMS> model) noexcept
	{
		m_models.emplace_back(std::move(model));
	}
	void SetMeshIndex(std::uint32_t index) noexcept
	{
		m_meshIndex = index;
	}

	[[nodiscard]]
	std::shared_ptr<ModelMS>& GetModel(size_t index) noexcept { return m_models[index]; }
	[[nodiscard]]
	const std::vector<std::shared_ptr<ModelMS>>& GetModels() const noexcept override { return m_models; }
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept override { return m_meshIndex; }

private:
	std::vector<std::shared_ptr<ModelMS>> m_models;
	std::uint32_t                         m_meshIndex;
};

// Mostly I should have different child classes of ModelBase with different functionalities.
// And then in the end before adding them to the renderer, add a wrapper based on the pipeline type.
template<class Derived>
class ModelBaseVSWrapper : public Derived, public ModelBaseVS
{
public:
	using Derived::Derived;

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final
	{
		return Derived::GetModelMatrix();
	}
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final
	{
		return Derived::GetModelOffset();
	}
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept final
	{
		return Derived::GetMaterialIndex();
	}
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override
	{
		return Derived::GetDiffuseIndex();
	}
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override
	{
		return Derived::GetDiffuseUVInfo();
	}
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override
	{
		return Derived::GetSpecularIndex();
	}
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override
	{
		return Derived::GetSpecularUVInfo();
	}
};
template<class Derived>
class ModelBaseMSWrapper : public Derived, public ModelBaseMS
{
public:
	using Derived::Derived;

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept final
	{
		return Derived::GetModelMatrix();
	}
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept final
	{
		return Derived::GetModelOffset();
	}
	[[nodiscard]]
	bool IsLightSource() const noexcept final
	{
		return Derived::IsLightSource();
	}
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept final
	{
		return Derived::GetMeshIndex();
	}
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept final
	{
		return Derived::GetMaterialIndex();
	}
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept override
	{
		return Derived::GetDiffuseIndex();
	}
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept override
	{
		return Derived::GetDiffuseUVInfo();
	}
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept override
	{
		return Derived::GetSpecularIndex();
	}
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept override
	{
		return Derived::GetSpecularUVInfo();
	}
};

template<typename T>
decltype(auto) GetVSModel(std::shared_ptr<ModelVS>& model)
{
	return dynamic_cast<ModelBaseVSWrapper<T>*>(model.get());
}
template<typename T>
decltype(auto) GetVSModel(std::shared_ptr<ModelMS>& model)
{
	return dynamic_cast<ModelBaseMSWrapper<T>*>(model.get());
}
#endif
