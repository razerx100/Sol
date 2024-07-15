#ifndef BASIC_MESH_BUNDLES_HPP_
#define BASIC_MESH_BUNDLES_HPP_
#include <MeshBundleBase.hpp>

class TriangleMesh : public MeshBundleBase
{
public:
	TriangleMesh();

	TriangleMesh(const TriangleMesh& other) noexcept : MeshBundleBase{ other } {}
	TriangleMesh& operator=(const TriangleMesh& other) noexcept
	{
		MeshBundleBase::operator=(other);

		return *this;
	}

	TriangleMesh(TriangleMesh&& other) noexcept : MeshBundleBase{ std::move(other) } {}
	TriangleMesh& operator=(TriangleMesh&& other) noexcept
	{
		MeshBundleBase::operator=(std::move(other));

		return *this;
	}
};

enum class CubeUVMode
{
	SingleColour,
	IndependentFaceTexture
};

class CubeMesh : public MeshBundleBase
{
public:
	CubeMesh(CubeUVMode uvMode);

private:
	void SetSingleColourUV() noexcept;
	void SetIndependentFaceTexUV() noexcept;

private:
	CubeUVMode m_uvMode;

public:
	CubeMesh(const CubeMesh& other) noexcept
		: MeshBundleBase{ other }, m_uvMode{ other.m_uvMode }
	{}
	CubeMesh& operator=(const CubeMesh& other) noexcept
	{
		MeshBundleBase::operator=(other);
		m_uvMode = other.m_uvMode;

		return *this;
	}

	CubeMesh(CubeMesh&& other) noexcept
		: MeshBundleBase{ std::move(other) }, m_uvMode{ other.m_uvMode }
	{}
	CubeMesh& operator=(CubeMesh&& other) noexcept
	{
		MeshBundleBase::operator=(std::move(other));
		m_uvMode = other.m_uvMode;

		return *this;
	}
};

class QuadMesh : public MeshBundleBase
{
public:
	QuadMesh();

	QuadMesh(const QuadMesh& other) noexcept
		: MeshBundleBase{ other }
	{}
	QuadMesh& operator=(const QuadMesh& other) noexcept
	{
		MeshBundleBase::operator=(other);

		return *this;
	}

	QuadMesh(QuadMesh&& other) noexcept
		: MeshBundleBase{ std::move(other) }
	{}
	QuadMesh& operator=(QuadMesh&& other) noexcept
	{
		MeshBundleBase::operator=(std::move(other));

		return *this;
	}
};

class SphereMesh : public MeshBundleBase
{
public:
	SphereMesh(std::uint32_t latDiv, std::uint32_t longDiv);

private:
	void CalculateNormals() noexcept;

private:
	std::uint32_t m_latitudeDivision;
	std::uint32_t m_longitudeDivision;

public:
	SphereMesh(const SphereMesh& other) noexcept
		: MeshBundleBase{ other },
		m_latitudeDivision{ other.m_latitudeDivision },
		m_longitudeDivision{ other.m_longitudeDivision }
	{}
	SphereMesh& operator=(const SphereMesh& other) noexcept
	{
		MeshBundleBase::operator=(other);
		m_latitudeDivision  = other.m_latitudeDivision;
		m_longitudeDivision = other.m_longitudeDivision;

		return *this;
	}

	SphereMesh(SphereMesh&& other) noexcept
		: MeshBundleBase{ std::move(other) },
		m_latitudeDivision{ other.m_latitudeDivision },
		m_longitudeDivision{ other.m_longitudeDivision }
	{}
	SphereMesh& operator=(SphereMesh&& other) noexcept
	{
		MeshBundleBase::operator=(std::move(other));
		m_latitudeDivision  = other.m_latitudeDivision;
		m_longitudeDivision = other.m_longitudeDivision;

		return *this;
	}
};
#endif
