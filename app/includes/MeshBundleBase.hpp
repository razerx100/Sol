#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <unordered_map>
#include <MeshBundle.hpp>
#include <SolMeshUtility.hpp>
#include <Renderer.hpp>
#include <assimp/scene.h>
#include <SceneMeshProcessor.hpp>

class MeshBundleTempIntermediate
{
public:
	virtual ~MeshBundleTempIntermediate() = default;

	virtual void AddMesh(Mesh&&) noexcept {}

	[[nodiscard]]
	virtual MeshBundleTemporaryData GenerateTemporaryData(bool meshShader) = 0;
};

class MeshBundleTempCustom : public MeshBundleTempIntermediate
{
public:
	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryData(bool meshShader) override;

	void AddMesh(Mesh&& mesh) noexcept override;

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

class MeshBundleTempAssimp : public MeshBundleTempIntermediate
{
public:
	MeshBundleTempAssimp() : m_meshProcessor{} {}

	[[nodiscard]]
	MeshBundleTemporaryData GenerateTemporaryData(bool meshShader) override;

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

class MeshBundleTemporaryImpl : public MeshBundleTemporary
{
public:
	MeshBundleTemporaryImpl(bool customTemp);

	void GenerateTemporaryData(bool meshShader) override;

	void AddMesh(Mesh&& mesh);

	void SetMeshBundle(std::shared_ptr<SceneProcessor> scene);

	// Vertex and Mesh
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return m_tempData.vertices;
	}
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetVertexIndices() const noexcept override
	{
		return m_tempData.indices;
	}
	[[nodiscard]]
	const MeshBundleTemporaryDetails& GetTemporaryBundleDetails() const noexcept override
	{
		return m_tempData.bundleDetails;
	}
	[[nodiscard]]
	MeshBundleTemporaryDetails&& GetTemporaryBundleDetails() noexcept override
	{
		return std::move(m_tempData.bundleDetails);
	}

	// Mesh only
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetPrimIndices() const noexcept override
	{
		return m_tempData.primIndices;
	}
	[[nodiscard]]
	const std::vector<MeshletDetails>& GetMeshletDetails() const noexcept override
	{
		return m_tempData.meshletDetails;
	}

private:
	MeshBundleTemporaryData                     m_tempData;
	std::unique_ptr<MeshBundleTempIntermediate> m_tempIntermediate;

public:
	MeshBundleTemporaryImpl(const MeshBundleTemporaryImpl&) = delete;
	MeshBundleTemporaryImpl& operator=(const MeshBundleTemporaryImpl&) = delete;

	MeshBundleTemporaryImpl(MeshBundleTemporaryImpl&& other) noexcept
		: m_tempData{ std::move(other.m_tempData) },
		m_tempIntermediate{ std::move(other.m_tempIntermediate) }
	{}
	MeshBundleTemporaryImpl& operator=(MeshBundleTemporaryImpl&& other) noexcept
	{
		m_tempData         = std::move(other.m_tempData);
		m_tempIntermediate = std::move(other.m_tempIntermediate);

		return *this;
	}
};

class MeshBundleImpl : public MeshBundle
{
public:
	MeshBundleImpl(bool customTemp)
		: m_temporaryData{ std::make_unique<MeshBundleTemporaryImpl>(customTemp) }
	{}

	void AddMesh(Mesh&& mesh);

	void SetMeshBundle(std::shared_ptr<SceneProcessor> scene);

	[[nodiscard]]
	std::unique_ptr<MeshBundleTemporary> MoveTemporaryData() override
	{
		return std::move(m_temporaryData);
	}

public:
	static void CalculateNormalsIndependentFaces(
		std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
	) noexcept;
	[[nodiscard]]
	static DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) noexcept;
	static void SetUVToVertices(
		std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
	) noexcept;

private:
	std::unique_ptr<MeshBundleTemporaryImpl> m_temporaryData;

public:
	MeshBundleImpl(const MeshBundleImpl&) = delete;
	MeshBundleImpl& operator=(const MeshBundleImpl&) = delete;

	MeshBundleImpl(MeshBundleImpl&& other) noexcept
		: m_temporaryData{ std::move(other.m_temporaryData) }
	{}
	MeshBundleImpl& operator=(MeshBundleImpl&& other) noexcept
	{
		m_temporaryData = std::move(other.m_temporaryData);

		return *this;
	}
};
#endif
