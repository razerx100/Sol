#ifndef SCENE_MESH_PROCESSOR_HPP_
#define SCENE_MESH_PROCESSOR_HPP_
#include <memory>
#include <vector>
#include <SceneProcessor.hpp>
#include <SolMeshUtility.hpp>

class SceneMeshProcessor
{
public:
	SceneMeshProcessor() : m_scene{} {}
	SceneMeshProcessor(std::shared_ptr<SceneProcessor> scene)
		: m_scene{ std::move(scene) }
	{}

	void SetSceneProcessor(std::shared_ptr<SceneProcessor> scene);

	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryMeshData(bool meshShader);

	void LoadMeshNodeDetails(
		std::vector<MeshPermanentDetails>& permanentDetails, std::vector<MeshNodeData>& meshNodeData
	);

private:
	[[nodiscard]]
	static MeshBundleTemporaryData GenerateMeshShaderData(aiScene const* scene);
	[[nodiscard]]
	static MeshBundleTemporaryData GenerateVertexShaderData(aiScene const* scene);

	static void ProcessMeshVS(aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData) noexcept;
	static void ProcessMeshMS(aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData) noexcept;

	static void ProcessMeshVertices(
		aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;
	static void ProcessMeshFaces(
		aiMesh* mesh, std::uint32_t vertexOffset, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;

	static void ProcessMeshNodeDetails(
		aiNode const* node, std::vector<MeshNodeData>& meshNodeData, aiMesh** meshes,
		std::uint32_t& childrenOffset, std::uint32_t& modelIndex
	);
	static void TraverseMeshHierarchyDetails(
		aiNode const* node,
		DirectX::XMMATRIX accumulatedTransform, std::vector<MeshPermanentDetails>& permanentDetails,
		std::vector<MeshNodeData>& meshNodeData, aiMesh** meshes, std::uint32_t& childrenOffset,
		std::uint32_t& modelIndex
	);

private:
	std::shared_ptr<SceneProcessor> m_scene;

public:
	SceneMeshProcessor(const SceneMeshProcessor&) = delete;
	SceneMeshProcessor& operator=(const SceneMeshProcessor&) = delete;

	SceneMeshProcessor(SceneMeshProcessor&& other) noexcept
		: m_scene{ std::move(other.m_scene) }
	{}
	SceneMeshProcessor& operator=(SceneMeshProcessor&& other) noexcept
	{
		m_scene = std::move(other.m_scene);

		return *this;
	}
};
#endif
