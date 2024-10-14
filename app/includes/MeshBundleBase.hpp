#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <unordered_map>
#include <MeshBundle.hpp>
#include <Renderer.hpp>

struct Mesh
{
	std::string                name;
	std::vector<Vertex>        vertices;
	std::vector<std::uint32_t> indices;
	std::vector<MeshBound>     bounds;
};

struct MeshExtraForMesh
{
	std::vector<std::uint32_t> primIndices;
	std::vector<Meshlet>       meshlets;
};

struct MeshDetails
{
	std::string   name;
	std::uint32_t elementCount;
	std::uint32_t elementOffset;
};

class MeshBundleBase
{
public:
	MeshBundleBase() : m_vertices{}, m_indices{}, m_bounds{} {}

	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept { return m_bounds; }
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept { return m_vertices; }
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept { return m_indices; }

	void AddMesh(Mesh&& mesh) noexcept;

	void CleanUpVertices() noexcept { m_vertices = std::vector<Vertex>{}; }

	static void CalculateNormalsIndependentFaces(
		std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
	) noexcept;

	static void SetUVToVertices(
		std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
	) noexcept;

private:
	[[nodiscard]]
	static DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) noexcept;

private:
	std::vector<Vertex>        m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::vector<MeshBound>     m_bounds;

public:
	MeshBundleBase(const MeshBundleBase& other) noexcept
		: m_vertices{ other.m_vertices }, m_indices{ other.m_indices }, m_bounds{ other.m_bounds }
	{}
	MeshBundleBase& operator=(const MeshBundleBase& other) noexcept
	{
		m_vertices = other.m_vertices;
		m_indices  = other.m_indices;
		m_bounds   = other.m_bounds;

		return *this;
	}

	MeshBundleBase(MeshBundleBase&& other) noexcept
		: m_vertices{ std::move(other.m_vertices) }, m_indices{ std::move(other.m_indices) },
		m_bounds{ std::move(other.m_bounds) }
	{}
	MeshBundleBase& operator=(MeshBundleBase&& other) noexcept
	{
		m_vertices = std::move(other.m_vertices);
		m_indices  = std::move(other.m_indices);
		m_bounds   = std::move(other.m_bounds);

		return *this;
	}
};

class MeshBundleBaseVS : public MeshBundleVS
{
public:
	MeshBundleBaseVS() : m_meshBase{} {}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept override
	{
		return m_meshBase.GetIndices();
	}
	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept override
	{
		return m_meshBase.GetBounds();
	};
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return m_meshBase.GetVertices();
	}

	void AddMesh(Mesh&& mesh) noexcept { m_meshBase.AddMesh(std::move(mesh)); }

private:
	MeshBundleBase m_meshBase;

public:
	MeshBundleBaseVS(const MeshBundleBaseVS& other) noexcept
		: m_meshBase{ other.m_meshBase }
	{}
	MeshBundleBaseVS& operator=(const MeshBundleBaseVS& other) noexcept
	{
		m_meshBase = other.m_meshBase;

		return *this;
	}

	MeshBundleBaseVS(MeshBundleBaseVS&& other) noexcept
		: m_meshBase{ std::move(other.m_meshBase) }
	{}
	MeshBundleBaseVS& operator=(MeshBundleBaseVS&& other) noexcept
	{
		m_meshBase = std::move(other.m_meshBase);

		return *this;
	}
};

class MeshBundleBaseMS : public MeshBundleMS
{
public:
	MeshBundleBaseMS() : m_meshBase{}, m_primIndices{}, m_meshlets{} {}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetVertexIndices() const noexcept override
	{
		return m_meshBase.GetIndices();
	}
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetPrimIndices() const noexcept override
	{
		return m_primIndices;
	}
	[[nodiscard]]
	const std::vector<Meshlet>& GetMeshlets() const noexcept override
	{
		return m_meshlets;
	}
	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept override
	{
		return m_meshBase.GetBounds();
	};
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return m_meshBase.GetVertices();
	}

	void AddMesh(Mesh&& mesh, MeshExtraForMesh&& extraMeshData) noexcept;

private:
	MeshBundleBase             m_meshBase;
	std::vector<std::uint32_t> m_primIndices;
	std::vector<Meshlet>       m_meshlets;

public:
	MeshBundleBaseMS(const MeshBundleBaseMS& other) noexcept
		: m_meshBase{ other.m_meshBase }, m_primIndices{ other.m_primIndices },
		m_meshlets{ other.m_meshlets }
	{}
	MeshBundleBaseMS& operator=(const MeshBundleBaseMS& other) noexcept
	{
		m_meshBase    = other.m_meshBase;
		m_primIndices = other.m_primIndices;
		m_meshlets    = other.m_meshlets;

		return *this;
	}

	MeshBundleBaseMS(MeshBundleBaseMS&& other) noexcept
		: m_meshBase{ std::move(other.m_meshBase) }, m_primIndices{ std::move(other.m_primIndices) },
		m_meshlets{ std::move(other.m_meshlets) }
	{}
	MeshBundleBaseMS& operator=(MeshBundleBaseMS&& other) noexcept
	{
		m_meshBase    = std::move(other.m_meshBase);
		m_primIndices = std::move(other.m_primIndices);
		m_meshlets    = std::move(other.m_meshlets);

		return *this;
	}
};

class MeshBundleGeneral
{
public:
	MeshBundleGeneral()
		: m_meshDetails{},
		m_bundleVS{ s_modelTypeVS ? std::make_unique<MeshBundleBaseVS>() : nullptr },
		m_bundleMS{ s_modelTypeVS ? nullptr : std::make_unique<MeshBundleBaseMS>() }
	{}

	static void SetMeshType(bool meshShader) noexcept { s_modelTypeVS = !meshShader; }

	MeshBundleGeneral& AddMesh(Mesh&& mesh) noexcept;

	[[nodiscard]]
	std::uint32_t SetMeshBundle(Renderer& renderer);
	[[nodiscard]]
	MeshDetails GetMeshDetails(const std::string& meshName) const noexcept;

private:
	std::vector<MeshDetails>          m_meshDetails;
	std::unique_ptr<MeshBundleBaseVS> m_bundleVS;
	std::unique_ptr<MeshBundleBaseMS> m_bundleMS;

	inline static bool s_modelTypeVS = true;

public:
	MeshBundleGeneral(const MeshBundleGeneral&) = delete;
	MeshBundleGeneral& operator=(const MeshBundleGeneral&) = delete;

	MeshBundleGeneral(MeshBundleGeneral&& other) noexcept
		: m_meshDetails{ std::move(other.m_meshDetails) },
		m_bundleVS{ std::move(other.m_bundleVS) },
		m_bundleMS{ std::move(other.m_bundleMS) }
	{}
	MeshBundleGeneral& operator=(MeshBundleGeneral&& other) noexcept
	{
		m_meshDetails = std::move(other.m_meshDetails);
		m_bundleVS    = std::move(other.m_bundleVS);
		m_bundleMS    = std::move(other.m_bundleMS);

		return *this;
	}
};

class MeshletMaker
{
public:
	MeshletMaker();

	void GenerateMeshlets(const Mesh& mesh);

	void LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices);

	[[nodiscard]]
	// It is not const, because it will move the data.
	MeshExtraForMesh GenerateExtraMeshData() noexcept;

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

private:
	union PrimitiveIndices
	{
		struct
		{
			std::uint32_t firstIndex  : 10u;
			std::uint32_t secondIndex : 10u;
			std::uint32_t thirdIndex  : 10u;
		} unpacked;
		std::uint32_t packed;
	};

	static constexpr size_t s_meshletVertexLimit    = 64u;
	static constexpr size_t s_meshletPrimitiveLimit = 126u;

private:
	std::vector<std::uint32_t> m_tempVertexIndices;
	std::vector<std::uint32_t> m_tempPrimitiveIndices;
	std::vector<std::uint32_t> m_vertexIndices;
	std::vector<std::uint32_t> m_primitiveIndices;
	std::vector<Meshlet>       m_meshlets;
};
#endif
