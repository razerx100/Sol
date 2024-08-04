#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <MeshBundle.hpp>

enum class BoundType
{
	Rectangle
};

class MeshBundleBase
{
public:
	MeshBundleBase(const std::string& name) : m_name{ name }, m_vertices{}, m_indices{}, m_bounds{}
	{}

	virtual ~MeshBundleBase() = default;

	void SetBounds(BoundType type) noexcept;

	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept { return m_bounds; }
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept { return m_vertices; }
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept { return m_indices; }

	void CleanUpVertices() noexcept { m_vertices = std::vector<Vertex>{}; }

	[[nodiscard]]
	const std::string& GetName() const noexcept { return m_name; }

protected:
	void CalculateNormalsIndependentFaces() noexcept;
	static void SetUVToVertices(
		std::vector<Vertex>& vertices, const std::vector<DirectX::XMFLOAT2>& uvs
	) noexcept;

private:
	[[nodiscard]]
	static DirectX::XMFLOAT3 GetFaceNormal(
		const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2,
		const DirectX::XMFLOAT3& position3
	) noexcept;

protected:
	std::string                m_name;
	std::vector<Vertex>        m_vertices;
	std::vector<std::uint32_t> m_indices;
	std::vector<MeshBound>     m_bounds;

public:
	MeshBundleBase(const MeshBundleBase& other) noexcept
		: m_name{ other.m_name }, m_vertices{ other.m_vertices },
		m_indices{ other.m_indices }, m_bounds{ other.m_bounds }
	{}
	MeshBundleBase& operator=(const MeshBundleBase& other) noexcept
	{
		m_name     = other.m_name;
		m_vertices = other.m_vertices;
		m_indices  = other.m_indices;
		m_bounds   = other.m_bounds;

		return *this;
	}

	MeshBundleBase(MeshBundleBase&& other) noexcept
		: m_name{ std::move(other.m_name) }, m_vertices{ std::move(other.m_vertices) },
		m_indices{ std::move(other.m_indices) }, m_bounds{ std::move(other.m_bounds) }
	{}
	MeshBundleBase& operator=(MeshBundleBase&& other) noexcept
	{
		m_name     = std::move(other.m_name);
		m_vertices = std::move(other.m_vertices);
		m_indices  = std::move(other.m_indices);
		m_bounds   = std::move(other.m_bounds);

		return *this;
	}
};

template<class Derived>
class MeshBaseVSWrapper : public Derived, public MeshBundleVS
{
public:
	using Derived::Derived;

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndices() const noexcept override
	{
		return Derived::GetIndices();
	}
	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept override
	{
		return Derived::GetBounds();
	};
	[[nodiscard]]
	const std::vector<Vertex>& GetVertices() const noexcept override
	{
		return Derived::GetVertices();
	}

	void CleanUpVertices() noexcept override
	{
		Derived::CleanUpVertices();
	}
};
#endif
