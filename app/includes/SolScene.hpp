#ifndef SOL_SCENE_HPP_
#define SOL_SCENE_HPP_
#include <SolMeshUtility.hpp>
#include <assimp/scene.h>
#include <SceneMeshProcessor.hpp>
#include <SceneMaterialProcessor.hpp>

class SolScene
{
public:
	SolScene() : m_sceneNodeData{}, m_meshMaterialDetails{} {}

	void SetSceneNodes(const SceneProcessor& sceneProcessor);

	void SetMeshMaterialDetails(
		const SceneProcessor& sceneProcessor, const SceneMaterialProcessor& materialProcessor
	);

	[[nodiscard]]
	const MeshMaterialDetails& GetMeshMaterialDetail(size_t index) const noexcept
	{
		return m_meshMaterialDetails[index];
	}
	[[nodiscard]]
	const std::vector<SceneNodeData>& GetSceneNodeData() const noexcept
	{
		return m_sceneNodeData;
	}
	[[nodiscard]]
	const std::vector<MeshMaterialDetails>& GetMeshMaterialDetails() const noexcept
	{
		return m_meshMaterialDetails;
	}

private:
	static void ProcessSceneNodeDetails(
		aiNode const* node, std::vector<SceneNodeData>& sceneNodeData,
		DirectX::XMMATRIX accumulatedTransform, aiMesh** meshes, std::uint32_t& childrenOffset,
		std::uint32_t& modelIndex
	);
	static void TraverseMeshHierarchyDetails(
		aiNode const* node, DirectX::XMMATRIX accumulatedTransform,
		std::vector<SceneNodeData>& sceneNodeData,
		aiMesh** meshes, std::uint32_t& childrenOffset, std::uint32_t& modelIndex
	);

private:
	std::vector<SceneNodeData>       m_sceneNodeData;
	std::vector<MeshMaterialDetails> m_meshMaterialDetails;

public:
	SolScene(const SolScene&) = delete;
	SolScene& operator=(const SolScene&) = delete;

	SolScene(SolScene&& other) noexcept
		: m_sceneNodeData{ std::move(other.m_sceneNodeData) },
		m_meshMaterialDetails{ std::move(other.m_meshMaterialDetails) }
	{}
	SolScene& operator=(SolScene&& other) noexcept
	{
		m_sceneNodeData       = std::move(other.m_sceneNodeData);
		m_meshMaterialDetails = std::move(other.m_meshMaterialDetails);

		return *this;
	}
};
#endif
