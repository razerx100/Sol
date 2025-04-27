#ifndef MODEL_SOL_HPP_
#define MODEL_SOL_HPP_
#include <cstdint>
#include <memory>
#include <vector>

#include <DirectXMath.h>

struct UVInfo
{
	float uOffset = 0.f;
	float vOffset = 0.f;
	float uScale  = 1.f;
	float vScale  = 1.f;
};

class ModelTransform
{
	struct BrokenDownMatrix
	{
		DirectX::XMMATRIX matrix;
		DirectX::XMFLOAT3 position;
		float             scale;
	};

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

	void Rotate(const DirectX::XMMATRIX& rotationMatrix) noexcept
	{
		m_modelMatrix *= rotationMatrix;
	}
	void Scale(const DirectX::XMMATRIX& scalingMatrix) noexcept
	{
		m_modelMatrix *= scalingMatrix;

		RecalculateScale();
	}

	[[nodiscard]]
	static DirectX::XMMATRIX GetRotationMatrix(
		const DirectX::XMVECTOR& rotationAxis, float angleRadian
	) noexcept {
		return DirectX::XMMatrixRotationAxis(rotationAxis, angleRadian);
	}

	[[nodiscard]]
	static DirectX::XMMATRIX GetScalingMatrix(float scaleX, float scaleY, float scaleZ) noexcept
	{
		return DirectX::XMMatrixScaling(scaleX, scaleY, scaleZ);
	}

	void Rotate(const DirectX::XMVECTOR& rotationAxis, float angleRadian) noexcept
	{
		Rotate(GetRotationMatrix(rotationAxis, angleRadian));
	}
	void Scale(float scale) noexcept
	{
		Scale(GetScalingMatrix(scale, scale, scale));
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
	void MultiplyAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
	{
		using namespace DirectX;

		BrokenDownMatrix brokenDownMatrix = BreakDownMatrix(matrix);

		{
			XMVECTOR newOffset
				= XMLoadFloat3(&m_modelOffset) + XMLoadFloat3(&brokenDownMatrix.position);

			XMStoreFloat3(&m_modelOffset, newOffset);
		}

		m_modelMatrix *= brokenDownMatrix.matrix;
		m_modelScale   = brokenDownMatrix.scale;
	}

	void SetAndBreakDownModelMatrix(const DirectX::XMMATRIX& matrix) noexcept
	{
		using namespace DirectX;

		BrokenDownMatrix brokenDownMatrix = BreakDownMatrix(matrix);

		m_modelOffset = brokenDownMatrix.position;
		m_modelMatrix = brokenDownMatrix.matrix;
		m_modelScale  = brokenDownMatrix.scale;
	}

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
	void RecalculateScale() noexcept
	{
		DirectX::XMVECTOR scale{};
		DirectX::XMVECTOR rotationQuat{};
		DirectX::XMVECTOR translation{};

		DirectX::XMMatrixDecompose(&scale, &rotationQuat, &translation, m_modelMatrix);

		// We are scaling all of the components by the same amount.
		m_modelScale = DirectX::XMVectorGetX(scale);
	}

	[[nodiscard]]
	static BrokenDownMatrix BreakDownMatrix(const DirectX::XMMATRIX& matrix) noexcept
	{
		using namespace DirectX;

		BrokenDownMatrix brokenDownMatrix{ .matrix = XMMatrixIdentity() };

		XMVECTOR scale{};
		XMVECTOR rotationQuat{};
		XMVECTOR translation{};

		XMMatrixDecompose(&scale, &rotationQuat, &translation, matrix);

		brokenDownMatrix.matrix *= XMMatrixScalingFromVector(scale);
		brokenDownMatrix.matrix *= XMMatrixRotationQuaternion(rotationQuat);

		XMStoreFloat3(&brokenDownMatrix.position, translation);

		brokenDownMatrix.scale = XMVectorGetX(scale);

		return brokenDownMatrix;
	}

private:
	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMFLOAT3 m_modelOffset;
	float             m_modelScale;
};

class ModelMaterial
{
public:
	ModelMaterial()
		: m_materialIndex{ 0u }, m_diffuseIndex{ 0u }, m_specularIndex{ 0u },
		m_diffuseUVInfo{ 0.f, 0.f, 1.f, 1.f }, m_specularUVInfo{ 0.f, 0.f, 1.f, 1.f }
	{}

	void SetMaterialIndex(std::uint32_t index) noexcept { m_materialIndex = index; }

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
	std::uint32_t GetMaterialIndex() const noexcept { return m_materialIndex; }
	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept { return m_diffuseIndex; }
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept { return m_diffuseUVInfo; }
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept { return m_specularIndex; }
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept { return m_specularUVInfo; }

private:
	std::uint32_t m_materialIndex;
	std::uint32_t m_diffuseIndex;
	std::uint32_t m_specularIndex;
	UVInfo        m_diffuseUVInfo;
	UVInfo        m_specularUVInfo;
};

// Represent a single drawable object.
class Model
{
public:
	Model()
		: m_transform{}, m_material{}, m_meshIndex{ 0u }, m_modelIndexInBuffer{ 0u },
		m_visible{ true }
	{}
	Model(float scale) : Model{}
	{
		Scale(scale);
	}

	void SetMeshIndex(std::uint32_t index) noexcept { m_meshIndex = index; }

	void Scale(float scale) noexcept
	{
		GetTransform().Scale(scale);
	}

	void SetVisibility(bool value) noexcept { m_visible = value; }

	[[nodiscard]]
	DirectX::XMMATRIX GetModelMatrix() const noexcept
	{
		return m_transform.GetModelMatrix();
	}
	[[nodiscard]]
	DirectX::XMFLOAT3 GetModelOffset() const noexcept
	{
		return m_transform.GetModelOffset();
	}
	[[nodiscard]]
	std::uint32_t GetMaterialIndex() const noexcept
	{
		return m_material.GetMaterialIndex();
	}
	[[nodiscard]]
	std::uint32_t GetMeshIndex() const noexcept { return m_meshIndex; }
	[[nodiscard]]
	float GetModelScale() const noexcept { return m_transform.GetModelScale(); }
	[[nodiscard]]
	bool IsVisible() const noexcept { return m_visible; }

	// This will be set from the renderers. Didn't want to set it from the consuming classes
	// but since every renderer will have model buffer index, this would be better.
	void SetModelIndexInBuffer(std::uint32_t index) noexcept
	{
		m_modelIndexInBuffer = index;
	}

	[[nodiscard]]
	std::uint32_t GetModelIndexInBuffer() const noexcept { return m_modelIndexInBuffer; }

	[[nodiscard]]
	std::uint32_t GetDiffuseIndex() const noexcept
	{
		return m_material.GetDiffuseIndex();
	}
	[[nodiscard]]
	UVInfo GetDiffuseUVInfo() const noexcept { return m_material.GetDiffuseUVInfo(); }
	[[nodiscard]]
	std::uint32_t GetSpecularIndex() const noexcept
	{
		return m_material.GetSpecularIndex();
	}
	[[nodiscard]]
	UVInfo GetSpecularUVInfo() const noexcept { return m_material.GetSpecularUVInfo(); }

	[[nodiscard]]
	auto&& GetTransform(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_transform);
	}

	[[nodiscard]]
	auto&& GetMaterial(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_material);
	}

	void CopyCharacteristics(const Model& other) noexcept
	{
		m_transform = other.m_transform;
		m_material  = other.m_material;
	}

private:
	ModelTransform m_transform;
	ModelMaterial  m_material;

	std::uint32_t m_meshIndex;
	std::uint32_t m_modelIndexInBuffer;
	bool          m_visible;

public:
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	Model(Model&& other) noexcept
		: m_transform{ std::move(other.m_transform) },
		m_material{ std::move(other.m_material) },
		m_meshIndex{ other.m_meshIndex },
		m_modelIndexInBuffer{ other.m_modelIndexInBuffer },
		m_visible{ other.m_visible }
	{}
	Model& operator=(Model&& other) noexcept
	{
		m_transform          = std::move(other.m_transform);
		m_material           = std::move(other.m_material);
		m_meshIndex          = other.m_meshIndex;
		m_modelIndexInBuffer = other.m_modelIndexInBuffer;
		m_visible            = other.m_visible;

		return *this;
	}
};

// Should contain all the models of a Model Bundle which have a certain pipeline.
class PipelineModelBundle
{
public:
	virtual ~PipelineModelBundle() = default;

	[[nodiscard]]
	virtual std::uint32_t GetPipelineIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetModelIndicesInBundle() const noexcept = 0;
};

// Should typically have a complex model with multiple models.
class ModelBundle
{
public:
	using ModelContainer_t    = std::vector<std::shared_ptr<Model>>;
	using PipelineContainer_t = std::vector<std::shared_ptr<PipelineModelBundle>>;

public:
	virtual ~ModelBundle() = default;

	[[nodiscard]]
	virtual std::uint32_t GetMeshBundleIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const PipelineContainer_t& GetPipelineBundles() const noexcept = 0;
	[[nodiscard]]
	virtual const ModelContainer_t& GetModels() const noexcept = 0;
	[[nodiscard]]
	virtual ModelContainer_t& GetModels() noexcept = 0;
};
#endif
