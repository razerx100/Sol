#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>

#include <SolMeshUtility.hpp>
#include <Model.hpp>

namespace Sol
{
class ModelBundleBase
{
	using ModelContainer_t    = ModelBundle::ModelContainer_t;
	using PipelineContainer_t = ModelBundle::PipelineContainer_t;

public:
	ModelBundleBase()
		: m_modelNodeData{}, m_models{}, m_pipelines{}, m_meshBundleIndex{ 0u }
	{}

	ModelBundleBase& AddModel(std::uint32_t pipelineIndex, float scale) noexcept;
	ModelBundleBase& AddModel(
		std::uint32_t pipelineIndex, std::shared_ptr<Model> model
	) noexcept;

	void ChangeModelPipeline(
		std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
		std::uint32_t newPipelineIndex
	) noexcept;

	void SetMeshBundleIndex(std::uint32_t index) noexcept
	{
		m_meshBundleIndex = index;
	}

	std::uint32_t AddPipeline(std::uint32_t pipelineIndex) noexcept;

	// Because of circular inclusion, can't include the BlinnPhoneTechnique header in this header.
	// So, have to do some template stuff to figure that out.
	template<class T>
	void SetMeshBundle(
		std::uint32_t meshBundleIndex, float modelScale, const T& scene
	) {
		const std::vector<SceneNodeData>& sceneNodeData = scene.GetSceneNodeData();
		const std::vector<MeshMaterialDetails>& meshMaterialDetails
			= scene.GetMeshMaterialDetails();

		SetMeshBundle(meshBundleIndex, modelScale, sceneNodeData, meshMaterialDetails);
	}

	void SetMeshBundle(
		std::uint32_t meshBundleIndex, float modelScale,
		const std::vector<SceneNodeData>& sceneNodeData,
		const std::vector<MeshMaterialDetails>& meshMaterialDetails
	);

	// Because of circular inclusion, can't include the BlinnPhoneTechnique header in this header.
	// So, have to do some template stuff to figure that out.
	template<class T>
	void ChangeMeshBundle(
		std::uint32_t meshBundleIndex, const T& scene,
		bool discardExistingTransformation = true
	) {
		const std::vector<SceneNodeData>& sceneNodeData = scene.GetSceneNodeData();
		const std::vector<MeshMaterialDetails>& meshMaterialDetails
			= scene.GetMeshMaterialDetails();

		ChangeMeshBundle(
			meshBundleIndex, sceneNodeData, meshMaterialDetails, discardExistingTransformation
		);
	}

	// Can only be changed if the new mesh count is the same as before.
	void ChangeMeshBundle(
		std::uint32_t meshBundleIndex,
		const std::vector<SceneNodeData>& sceneNodeData,
		const std::vector<MeshMaterialDetails>& meshMaterialDetails,
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
	auto&& GetModels(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_models);
	}
	[[nodiscard]]
	auto&& GetPipelines(this auto&& self) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_pipelines);
	}

	[[nodiscard]]
	auto&& GetModel(this auto&& self, size_t index) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_models[index]);
	}
	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept { return m_meshBundleIndex; }

private:
	void SetModels(float modelScale, const std::vector<SceneNodeData>& sceneNodeData);

	// Will add a new one if not available.
	[[nodiscard]]
	size_t GetLocalPipelineIndex(std::uint32_t pipelineIndex);

	static void SetMaterial(
		ModelMaterial& material, const MeshMaterialDetails& materialDetails
	) noexcept;

private:
	std::vector<ModelNodeData> m_modelNodeData;
	ModelContainer_t           m_models;
	PipelineContainer_t        m_pipelines;
	std::uint32_t              m_meshBundleIndex;

public:
	ModelBundleBase(const ModelBundleBase&) = delete;
	ModelBundleBase& operator=(const ModelBundleBase&) = delete;

	ModelBundleBase(ModelBundleBase&& other) noexcept
		: m_modelNodeData{ std::move(other.m_modelNodeData) },
		m_models{ std::move(other.m_models) },
		m_pipelines{ std::move(other.m_pipelines) },
		m_meshBundleIndex{ other.m_meshBundleIndex }
	{}
	ModelBundleBase& operator=(ModelBundleBase&& other) noexcept
	{
		m_modelNodeData   = std::move(other.m_modelNodeData);
		m_models          = std::move(other.m_models);
		m_pipelines       = std::move(other.m_pipelines);
		m_meshBundleIndex = other.m_meshBundleIndex;

		return *this;
	}
};

class ModelBundleImpl : public ModelBundle
{
public:
	ModelBundleImpl(std::shared_ptr<ModelBundleBase> modelBundleBase)
		: ModelBundle{}, m_modelBundleBase{ std::move(modelBundleBase) }
	{}

	[[nodiscard]]
	const ModelContainer_t& GetModels() const noexcept override
	{
		return m_modelBundleBase->GetModels();
	}
	[[nodiscard]]
	ModelContainer_t& GetModels() noexcept override
	{
		return m_modelBundleBase->GetModels();
	}
	[[nodiscard]]
	const PipelineContainer_t& GetPipelineBundles() const noexcept override
	{
		return m_modelBundleBase->GetPipelines();
	}

	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept override
	{
		return m_modelBundleBase->GetMeshBundleIndex();
	}

private:
	std::shared_ptr<ModelBundleBase> m_modelBundleBase;

public:
	ModelBundleImpl(const ModelBundleImpl&) = delete;
	ModelBundleImpl& operator=(const ModelBundleImpl&) = delete;

	ModelBundleImpl(ModelBundleImpl&& other) noexcept
		: m_modelBundleBase{ std::move(other.m_modelBundleBase) }
	{}
	ModelBundleImpl& operator=(ModelBundleImpl&& other) noexcept
	{
		m_modelBundleBase = std::move(other.m_modelBundleBase);

		return *this;
	}
};
}
#endif
