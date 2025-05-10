#ifndef MODEL_BASE_HPP_
#define MODEL_BASE_HPP_
#include <vector>
#include <string>
#include <array>

#include <SolMeshUtility.hpp>
#include <ModelBundle.hpp>
#include <ModelContainer.hpp>

namespace Sol
{
class ModelBundleBase
{
public:
	ModelBundleBase()
		: m_modelNodeData{}, m_modelBundle{ std::make_shared<ModelBundle>() }
	{}
	ModelBundleBase(std::shared_ptr<ModelBundle> modelBundle)
		: m_modelNodeData{}, m_modelBundle{ std::move(modelBundle) }
	{}

	std::uint32_t AddModel(std::uint32_t pipelineIndex, float scale) noexcept;
	std::uint32_t AddModel(std::uint32_t pipelineIndex, Model&& model) noexcept;

	void ChangeModelPipeline(
		std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
		std::uint32_t newPipelineIndex
	) noexcept;

	void SetMeshBundleIndex(std::uint32_t index) noexcept
	{
		m_modelBundle->SetMeshBundleIndex(index);
	}

	void SetModelContainer(std::shared_ptr<ModelContainer> modelContainer) noexcept
	{
		m_modelBundle->SetModelContainer(std::move(modelContainer));
	}

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
	auto&& GetModel(this auto&& self, size_t index) noexcept
	{
		return std::forward_like<decltype(self)>(self.m_modelBundle->GetModel(index));
	}
	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept
	{
		return m_modelBundle->GetMeshBundleIndex();
	}

	[[nodiscard]]
	size_t GetModelCount() const noexcept { return m_modelBundle->GetModelCount(); }

	[[nodiscard]]
	std::uint32_t GetIndexInContainer(size_t localIndex) noexcept
	{
		return m_modelBundle->GetIndexInContainer(localIndex);
	}

	[[nodiscard]]
	std::shared_ptr<ModelBundle> GetModelBundle() const noexcept { return m_modelBundle; }

private:
	void SetModels(float modelScale, const std::vector<SceneNodeData>& sceneNodeData);

	static void SetMaterial(
		ModelMaterial& material, const MeshMaterialDetails& materialDetails
	) noexcept;

private:
	std::vector<ModelNodeData>   m_modelNodeData;
	std::shared_ptr<ModelBundle> m_modelBundle;

public:
	ModelBundleBase(const ModelBundleBase&) = delete;
	ModelBundleBase& operator=(const ModelBundleBase&) = delete;

	ModelBundleBase(ModelBundleBase&& other) noexcept
		: m_modelNodeData{ std::move(other.m_modelNodeData) },
		m_modelBundle{ std::move(other.m_modelBundle) }
	{}
	ModelBundleBase& operator=(ModelBundleBase&& other) noexcept
	{
		m_modelNodeData = std::move(other.m_modelNodeData);
		m_modelBundle   = std::move(other.m_modelBundle);

		return *this;
	}
};
}
#endif
