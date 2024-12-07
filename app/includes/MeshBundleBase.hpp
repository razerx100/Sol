#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <unordered_map>
#include <MeshBundle.hpp>
#include <Renderer.hpp>
#include <assimp/scene.h>

struct Mesh
{
	std::vector<Vertex>        vertices;
	std::vector<std::uint32_t> indices;
};

struct MeshExtraForMesh
{
	std::vector<std::uint32_t>  primIndices;
	std::vector<MeshletDetails> meshletDetails;
};

struct MeshBundleData
{
	std::vector<Vertex>         vertices;
	std::vector<std::uint32_t>  indices;
	std::vector<std::uint32_t>  primIndices;
	std::vector<MeshletDetails> meshletDetails;
	MeshBundleDetails           bundleDetails;
};

struct ModelChildren
{
	std::uint32_t count;
	std::uint32_t startingIndex;
};

struct MeshPermanentDetails
{
	DirectX::XMMATRIX worldMatrix;
};

class MeshBundleTempIntermediate
{
public:
	virtual ~MeshBundleTempIntermediate() = default;

	virtual void AddMesh(Mesh&&) noexcept {}

	[[nodiscard]]
	virtual MeshBundleData GenerateTemporaryData(bool meshShader) = 0;
};

class MeshBundleTempCustom : public MeshBundleTempIntermediate
{
public:
	[[nodiscard]]
	MeshBundleData GenerateTemporaryData(bool meshShader) override;

	void AddMesh(Mesh&& mesh) noexcept;

private:
	static void GenerateMeshShaderData(Mesh& mesh, MeshBundleData& meshBundleData);
	static void GenerateVertexShaderData(Mesh& mesh, MeshBundleData& meshBundleData);

private:
	std::vector<Mesh> m_tempMeshes;
};

class MeshBundleTempAssimp : public MeshBundleTempIntermediate
{
public:
	MeshBundleTempAssimp() : m_tempScene{ nullptr } {}
	~MeshBundleTempAssimp() noexcept;

	// Use the assimp mesh indices to load the meshes in a BFS order.
	[[nodiscard]]
	MeshBundleData GenerateTemporaryData(bool meshShader) override;

	void SetMeshBundle(const std::string& fileName);

	void FillMeshHierarchyDetails(
		std::vector<MeshPermanentDetails>& permananeDetails, std::vector<ModelChildren>& childrenData
	);

private:
	[[nodiscard]]
	static MeshBundleData GenerateMeshShaderData(aiScene const* scene);
	[[nodiscard]]
	static MeshBundleData GenerateVertexShaderData(aiScene const* scene);

	static void ProcessMeshVertices(aiMesh* mesh, MeshBundleData& meshBundleData) noexcept;
	static void ProcessMeshFaces(aiMesh* mesh, MeshBundleData& meshBundleData) noexcept;

	static void ProcessMeshNodeVS(aiNode const* node, aiMesh** meshes, MeshBundleData& meshBundleData);
	static void TraverseMeshByLevelVS(aiNode const* node, aiMesh** meshes, MeshBundleData& meshBundleData);

	static void ProcessMeshNodeMS(aiNode const* node, aiMesh** meshes, MeshBundleData& meshBundleData);
	static void TraverseMeshByLevelMS(aiNode const* node, aiMesh** meshes, MeshBundleData& meshBundleData);

	static void ProcessMeshChildrenDetails(
		aiNode const* node, std::vector<ModelChildren>& childrenData, std::uint32_t& childrenOffset
	);
	static void TraverseMeshHierarchyDetails(
		aiNode const* node,
		DirectX::XMMATRIX accumulatedTransform, std::vector<MeshPermanentDetails>& permanentDetails,
		std::vector<ModelChildren>& childrenData, std::uint32_t& childrenOffset
	);

private:
	aiScene const* m_tempScene;

public:
	MeshBundleTempAssimp(const MeshBundleTempAssimp&) = delete;
	MeshBundleTempAssimp& operator=(const MeshBundleTempAssimp&) = delete;

	MeshBundleTempAssimp(MeshBundleTempAssimp&& other) noexcept
		: m_tempScene{ std::exchange(other.m_tempScene, nullptr) }
	{}
	MeshBundleTempAssimp& operator=(MeshBundleTempAssimp&& other) noexcept
	{
		m_tempScene = std::exchange(other.m_tempScene, nullptr);

		return *this;
	}
};

class MeshBundleTemporaryImpl : public MeshBundleTemporary
{
public:
	MeshBundleTemporaryImpl(bool customTemp);

	void GenerateTemporaryData(bool meshShader) override;

	void AddMesh(Mesh&& mesh);

	void SetMeshBundle(
		const std::string& fileName,
		std::vector<MeshPermanentDetails>& permanentDetails, std::vector<ModelChildren>& childrenData
	);

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
	const MeshBundleDetails& GetBundleDetails() const noexcept override
	{
		return m_tempData.bundleDetails;
	}
	[[nodiscard]]
	MeshBundleDetails&& GetBundleDetails() noexcept override
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
	MeshBundleData                              m_tempData;
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
		: m_childrenData{}, m_permanentDetails{},
		m_temporaryData{ std::make_unique<MeshBundleTemporaryImpl>(customTemp) }
	{}

	void AddMesh(Mesh&& mesh);

	void SetMeshBundle(const std::string& fileName);

	[[nodiscard]]
	std::unique_ptr<MeshBundleTemporary> MoveTemporaryData() override
	{
		return std::move(m_temporaryData);
	}
	[[nodiscard]]
	const MeshPermanentDetails& GetPermanentDetail(size_t index) const noexcept
	{
		return m_permanentDetails[index];
	}
	[[nodiscard]]
	// The first one will be the root, if the models are hierarchical.
	const std::vector<ModelChildren>& GetModelChildrenData() const noexcept
	{
		return m_childrenData;
	}
	[[nodiscard]]
	const std::vector<MeshPermanentDetails>& GetPermanentDetails() const noexcept
	{
		return m_permanentDetails;
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
	std::vector<ModelChildren>               m_childrenData;
	std::vector<MeshPermanentDetails>        m_permanentDetails;
	std::unique_ptr<MeshBundleTemporaryImpl> m_temporaryData;

public:
	MeshBundleImpl(const MeshBundleImpl&) = delete;
	MeshBundleImpl& operator=(const MeshBundleImpl&) = delete;

	MeshBundleImpl(MeshBundleImpl&& other) noexcept
		: m_childrenData{ std::move(other.m_childrenData) },
		m_permanentDetails{std::move(other.m_permanentDetails)},
		m_temporaryData{ std::move(other.m_temporaryData) }
	{}
	MeshBundleImpl& operator=(MeshBundleImpl&& other) noexcept
	{
		m_childrenData     = std::move(other.m_childrenData);
		m_permanentDetails = std::move(other.m_permanentDetails);
		m_temporaryData    = std::move(other.m_temporaryData);

		return *this;
	}
};

class MeshletMaker
{
public:
	struct PrimitiveIndicesUnpacked
	{
		std::uint32_t firstIndex  : 10u;
		std::uint32_t secondIndex : 10u;
		std::uint32_t thirdIndex  : 10u;
	};

public:
	MeshletMaker();

	void GenerateMeshlets(const Mesh& mesh) noexcept;
	void GenerateMeshlets(aiMesh const* mesh) noexcept;

	void LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices) noexcept;

	[[nodiscard]]
	// It is not const, because it will move the data.
	MeshExtraForMesh GenerateExtraMeshData() noexcept;

	[[nodiscard]]
	static PrimitiveIndicesUnpacked UnpackPrim(std::uint32_t packedIndices) noexcept;

private:
	[[nodiscard]]
	static bool IsInMap(
		const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::uint32_t vIndex
	) noexcept;
	[[nodiscard]]
	static std::uint32_t GetPrimIndex(
		std::uint32_t vIndex, std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::vector<std::uint32_t>& vertexIndices
	) noexcept;
	[[nodiscard]]
	static std::uint32_t GetExtraVertexCount(
		const std::unordered_map<std::uint32_t, std::uint32_t>& vertexIndicesMap,
		std::uint32_t primIndex1, std::uint32_t primIndex2, std::uint32_t primIndex3
	) noexcept;

	[[nodiscard]]
	size_t MakeMeshlet(const std::vector<std::uint32_t>& indices, size_t startingIndex) noexcept;
	[[nodiscard]]
	size_t MakeMeshlet(aiFace* faces, size_t faceCount, size_t startingIndex) noexcept;

private:
	static constexpr size_t s_meshletVertexLimit    = 64u;
	static constexpr size_t s_meshletPrimitiveLimit = 126u;

private:
	std::vector<std::uint32_t>  m_tempVertexIndices;
	std::vector<std::uint32_t>  m_tempPrimitiveIndices;
	std::vector<std::uint32_t>  m_vertexIndices;
	std::vector<std::uint32_t>  m_primitiveIndices;
	std::vector<MeshletDetails> m_meshletDetails;
};
#endif
