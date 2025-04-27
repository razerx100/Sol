#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <unordered_map>
#include <MeshBundle.hpp>
#include <SolMeshUtility.hpp>
#include <assimp/scene.h>
#include <SceneMeshProcessor.hpp>

namespace Sol
{
class MeshBundleTempCustom
{
public:
	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryData(bool meshShader);

	void AddMesh(Mesh&& mesh) noexcept;

private:
	static void GenerateMeshShaderData(
		std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
	);
	static void GenerateVertexShaderData(
		std::vector<Mesh>& meshes, MeshBundleTemporaryData& meshBundleTemporaryData
	);

	static void ProcessMeshVS(
		Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;
	static void ProcessMeshMS(
		Mesh& mesh, MeshBundleTemporaryData& meshBundleTemporaryData
	) noexcept;

private:
	std::vector<Mesh> m_tempMeshes;
};

class MeshBundleTempAssimp
{
public:
	MeshBundleTempAssimp() : m_meshProcessor{} {}

	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryData(bool meshShader);

	void SetSceneProcessor(std::shared_ptr<SceneProcessor> scene);

private:
	SceneMeshProcessor m_meshProcessor;

public:
	MeshBundleTempAssimp(const MeshBundleTempAssimp&) = delete;
	MeshBundleTempAssimp& operator=(const MeshBundleTempAssimp&) = delete;

	MeshBundleTempAssimp(MeshBundleTempAssimp&& other) noexcept
		: m_meshProcessor{ std::move(other.m_meshProcessor) }
	{}
	MeshBundleTempAssimp& operator=(MeshBundleTempAssimp&& other) noexcept
	{
		m_meshProcessor = std::move(other.m_meshProcessor);

		return *this;
	}
};
}
#endif
