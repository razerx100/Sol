#ifndef MESH_BUNDLE_BASE_HPP_
#define MESH_BUNDLE_BASE_HPP_
#include <string>
#include <MeshBundle.hpp>

class MeshBundleBase
{
public:
	MeshBundleBase(const std::string& name);

	virtual ~MeshBundleBase() = default;

	[[nodiscard]]
	const std::vector<MeshBound>& GetBounds() const noexcept
	{
		static std::vector<MeshBound> meshBounds{};
		return meshBounds;
	}
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

public:
	MeshBundleBase(const MeshBundleBase& other) noexcept
		: m_name{ other.m_name }, m_vertices{ other.m_vertices },
		m_indices{ other.m_indices }
	{}
	MeshBundleBase& operator=(const MeshBundleBase& other) noexcept
	{
		m_name     = other.m_name;
		m_vertices = other.m_vertices;
		m_indices  = other.m_indices;

		return *this;
	}

	MeshBundleBase(MeshBundleBase&& other) noexcept
		: m_name{ std::move(other.m_name) }, m_vertices{ std::move(other.m_vertices) },
		m_indices{ std::move(other.m_indices) }
	{}
	MeshBundleBase& operator=(MeshBundleBase&& other) noexcept
	{
		m_name     = std::move(other.m_name);
		m_vertices = std::move(other.m_vertices);
		m_indices  = std::move(other.m_indices);

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
