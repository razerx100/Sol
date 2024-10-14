#ifndef BASIC_MESH_BUNDLES_HPP_
#define BASIC_MESH_BUNDLES_HPP_
#include <MeshBundleBase.hpp>

class TriangleMesh
{
public:
	static void SetMesh(Mesh& mesh) noexcept;

	[[nodiscard]]
	static std::string GetName() noexcept { return "Triangle"; }
};

enum class CubeUVMode
{
	SingleColour,
	IndependentFaceTexture
};

class CubeMesh
{
public:
	static void SetMesh(Mesh& mesh, CubeUVMode uvMode) noexcept;

	[[nodiscard]]
	static std::string GetName(CubeUVMode uvMode) noexcept;

private:
	static void SetSingleColourUV(std::vector<Vertex>& vertices) noexcept;
	static void SetIndependentFaceTexUV(std::vector<Vertex>& vertices) noexcept;
};

class QuadMesh
{
public:
	static void SetMesh(Mesh& mesh) noexcept;
};

class SphereMesh
{
public:
	SphereMesh(std::uint32_t latDiv, std::uint32_t longDiv)
		: m_latitudeDivision{ latDiv }, m_longitudeDivision{ longDiv }
	{}

	void SetMesh(Mesh& mesh) noexcept;

	[[nodiscard]]
	static std::string GetName(std::uint32_t latDiv, std::uint32_t longDiv) noexcept;

private:
	static void CalculateNormals(std::vector<Vertex>& vertices) noexcept;

private:
	std::uint32_t m_latitudeDivision;
	std::uint32_t m_longitudeDivision;

public:
	SphereMesh(const SphereMesh& other) noexcept
		: m_latitudeDivision{ other.m_latitudeDivision },
		m_longitudeDivision{ other.m_longitudeDivision }
	{}
	SphereMesh& operator=(const SphereMesh& other) noexcept
	{
		m_latitudeDivision  = other.m_latitudeDivision;
		m_longitudeDivision = other.m_longitudeDivision;

		return *this;
	}

	SphereMesh(SphereMesh&& other) noexcept
		: m_latitudeDivision{ other.m_latitudeDivision },
		m_longitudeDivision{ other.m_longitudeDivision }
	{}
	SphereMesh& operator=(SphereMesh&& other) noexcept
	{
		m_latitudeDivision  = other.m_latitudeDivision;
		m_longitudeDivision = other.m_longitudeDivision;

		return *this;
	}
};
#endif
