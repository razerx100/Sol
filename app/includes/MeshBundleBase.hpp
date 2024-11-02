#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <unordered_map>
#include <MeshBundle.hpp>
#include <Renderer.hpp>

struct Mesh
{
	std::vector<Vertex>        vertices;
	std::vector<std::uint32_t> indices;
	std::vector<MeshBound>     bounds;
};

struct MeshExtraForMesh
{
	std::vector<std::uint32_t> primIndices;
	std::vector<Meshlet>       meshlets;
};

class MeshBundleBase
{
public:
	MeshBundleBase()
		: m_vertices{}, m_indices{}, m_primIndices{}, m_meshlets{}, m_bounds{},
		m_bundleDetails{}
	{}

	MeshBundleBase& AddMesh(Mesh&& mesh) noexcept;

	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept { return m_bounds; }
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept { return m_vertices; }
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept { return m_indices; }

	static void CalculateNormalsIndependentFaces(
		std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices
	) noexcept;

	static void SetUVToVertices(
		std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
	) noexcept;

	static void SetMeshTypeVS(bool value) noexcept { s_meshTypeVS = value; }

	[[nodiscard]]
	static std::uint32_t SetMeshBundle(Renderer& renderer, MeshBundleBase&& meshBundle);

private:
	void AddMesh(Mesh&& mesh, const MeshDetails& meshDetails) noexcept;
	void AddMesh(
		Mesh&& mesh, MeshExtraForMesh&& extraMeshData, const MeshDetails& meshDetails
	) noexcept;

	[[nodiscard]]
	static DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) noexcept;

private:
	std::vector<Vertex>        m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::vector<std::uint32_t> m_primIndices;
	std::vector<Meshlet>       m_meshlets;
	std::vector<MeshBound>     m_bounds;
	MeshBundleDetails          m_bundleDetails;

	static bool                s_meshTypeVS;

public:
	MeshBundleBase(const MeshBundleBase& other) noexcept
		: m_vertices{ other.m_vertices }, m_indices{ other.m_indices },
		m_primIndices{ other.m_primIndices }, m_meshlets{ other.m_meshlets },
		m_bounds{ other.m_bounds }, m_bundleDetails{ other.m_bundleDetails }
	{}
	MeshBundleBase& operator=(const MeshBundleBase& other) noexcept
	{
		m_vertices      = other.m_vertices;
		m_indices       = other.m_indices;
		m_primIndices   = other.m_primIndices;
		m_meshlets      = other.m_meshlets;
		m_bounds        = other.m_bounds;
		m_bundleDetails = other.m_bundleDetails;

		return *this;
	}

	MeshBundleBase(MeshBundleBase&& other) noexcept
		: m_vertices{ std::move(other.m_vertices) }, m_indices{ std::move(other.m_indices) },
		m_primIndices{ std::move(other.m_primIndices) }, m_meshlets{ std::move(other.m_meshlets) },
		m_bounds{ std::move(other.m_bounds) }, m_bundleDetails{ std::move(other.m_bundleDetails) }
	{}
	MeshBundleBase& operator=(MeshBundleBase&& other) noexcept
	{
		m_vertices      = std::move(other.m_vertices);
		m_indices       = std::move(other.m_indices);
		m_primIndices   = std::move(other.m_primIndices);
		m_meshlets      = std::move(other.m_meshlets);
		m_bounds        = std::move(other.m_bounds);
		m_bundleDetails = std::move(other.m_bundleDetails);

		return *this;
	}
};

class MeshBundleBaseVS : public MeshBundleVS
{
	friend MeshBundleBase;

public:
	MeshBundleBaseVS()
		: MeshBundleVS{}, m_vertices{}, m_indices{}, m_bounds{}, m_bundleDetails{}
	{}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept override
	{
		return m_indices;
	}
	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept override
	{
		return m_bounds;
	};
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return m_vertices;
	}

	[[nodiscard]]
	MeshBundleDetails&& GetBundleDetails() noexcept override
	{
		return std::move(m_bundleDetails);
	}

private:
	std::vector<Vertex>        m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::vector<MeshBound>     m_bounds;
	MeshBundleDetails          m_bundleDetails;

public:
	MeshBundleBaseVS(const MeshBundleBaseVS& other) noexcept
		: MeshBundleVS{ other }, m_vertices{ other.m_vertices }, m_indices{ other.m_indices },
		m_bounds{ other.m_bounds }, m_bundleDetails{ other.m_bundleDetails }
	{}
	MeshBundleBaseVS& operator=(const MeshBundleBaseVS& other) noexcept
	{
		MeshBundleVS::operator=(other);
		m_vertices            = other.m_vertices;
		m_indices             = other.m_indices;
		m_bounds              = other.m_bounds;
		m_bundleDetails       = other.m_bundleDetails;

		return *this;
	}

	MeshBundleBaseVS(MeshBundleBaseVS&& other) noexcept
		: MeshBundleVS{ std::move(other) }, m_vertices{ std::move(other.m_vertices) },
		m_indices{ std::move(other.m_indices) }, m_bounds{ std::move(other.m_bounds) },
		m_bundleDetails{ std::move(other.m_bundleDetails) }
	{}
	MeshBundleBaseVS& operator=(MeshBundleBaseVS&& other) noexcept
	{
		MeshBundleVS::operator=(std::move(other));
		m_vertices            = std::move(other.m_vertices);
		m_indices             = std::move(other.m_indices);
		m_bounds              = std::move(other.m_bounds);
		m_bundleDetails       = std::move(other.m_bundleDetails);

		return *this;
	}
};

class MeshBundleBaseMS : public MeshBundleMS
{
	friend MeshBundleBase;

public:
	MeshBundleBaseMS()
		: MeshBundleMS{}, m_vertices{}, m_indices{}, m_primIndices{}, m_meshlets{}, m_bounds{},
		m_bundleDetails{}
	{}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetVertexIndices() const noexcept override
	{
		return m_indices;
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
		return m_bounds;
	};
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return m_vertices;
	}

	[[nodiscard]]
	MeshBundleDetails&& GetBundleDetails() noexcept override
	{
		return std::move(m_bundleDetails);
	}

private:
	std::vector<Vertex>        m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::vector<std::uint32_t> m_primIndices;
	std::vector<Meshlet>       m_meshlets;
	std::vector<MeshBound>     m_bounds;
	MeshBundleDetails          m_bundleDetails;

public:
	MeshBundleBaseMS(const MeshBundleBaseMS& other) noexcept
		: MeshBundleMS{ other }, m_vertices{ other.m_vertices }, m_indices{ other.m_indices },
		m_primIndices{ other.m_primIndices }, m_meshlets{ other.m_meshlets },
		m_bounds{ other.m_bounds }, m_bundleDetails{ other.m_bundleDetails }
	{}
	MeshBundleBaseMS& operator=(const MeshBundleBaseMS& other) noexcept
	{
		MeshBundleMS::operator=(other);
		m_vertices            = other.m_vertices;
		m_indices             = other.m_indices;
		m_primIndices         = other.m_primIndices;
		m_meshlets            = other.m_meshlets;
		m_bounds              = other.m_bounds;
		m_bundleDetails       = other.m_bundleDetails;

		return *this;
	}

	MeshBundleBaseMS(MeshBundleBaseMS&& other) noexcept
		: MeshBundleMS{ std::move(other) }, m_vertices{ std::move(other.m_vertices) },
		m_indices{ std::move(other.m_indices) }, m_primIndices{ std::move(other.m_primIndices) },
		m_meshlets{ std::move(other.m_meshlets) }, m_bounds{ std::move(other.m_bounds) },
		m_bundleDetails{ std::move(other.m_bundleDetails) }
	{}
	MeshBundleBaseMS& operator=(MeshBundleBaseMS&& other) noexcept
	{
		MeshBundleMS::operator=(std::move(other));
		m_vertices            = std::move(other.m_vertices);
		m_indices             = std::move(other.m_indices);
		m_primIndices         = std::move(other.m_primIndices);
		m_meshlets            = std::move(other.m_meshlets);
		m_bounds              = std::move(other.m_bounds);
		m_bundleDetails       = std::move(other.m_bundleDetails);

		return *this;
	}
};

class MeshletMaker
{
public:
	MeshletMaker();

	void GenerateMeshlets(const Mesh& mesh) noexcept;

	void LoadVertexIndices(std::vector<std::uint32_t>& vertexIndices) noexcept;

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
