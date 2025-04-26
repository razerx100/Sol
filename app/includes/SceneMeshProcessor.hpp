#ifndef SCENE_MESH_PROCESSOR_HPP_
#define SCENE_MESH_PROCESSOR_HPP_
#include <memory>
#include <vector>
#include <SceneProcessor.hpp>
#include <SolMeshUtility.hpp>
#include <GLTFObject.hpp>

namespace Sol
{
class SceneMeshProcessor
{
public:
	SceneMeshProcessor() : m_scene{} {}
	SceneMeshProcessor(std::shared_ptr<SceneProcessor> scene) : m_scene{ std::move(scene) } {}

	void SetSceneProcessor(std::shared_ptr<SceneProcessor> scene);

	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryMeshData(bool meshShader);

private:
	[[nodiscard]]
	static MeshBundleTemporaryData GenerateMeshShaderData(aiScene const* scene);
	[[nodiscard]]
	static MeshBundleTemporaryData GenerateVertexShaderData(aiScene const* scene);

	static void ProcessMeshVS(
		aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;
	static void ProcessMeshMS(
		aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;

	static void ProcessMeshVertices(
		aiMesh* mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;
	static void ProcessMeshFaces(
		aiMesh* mesh, std::uint32_t vertexOffset, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;

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

namespace SceneMeshProcessor1
{
	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryMeshData(const GLTFObject& gltfObj, bool meshShader);
};
}
#endif
