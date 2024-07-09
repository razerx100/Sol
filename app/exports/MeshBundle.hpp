#ifndef MESH_BUNDLE_SOL_HPP_
#define MESH_BUNDLE_SOL_HPP_
#include <cstdint>
#include <vector>

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct MeshBound
{
	DirectX::XMFLOAT3 position;
};

class MeshBundle
{
public:
	virtual ~MeshBundle() = default;

	[[nodiscard]]
	virtual const std::vector<MeshBound>& GetBounds() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<Vertex>& GetVertices() const noexcept = 0;

	virtual void CleanUpVertices() noexcept = 0;
};

class MeshBundleVS : public MeshBundle
{
public:
	virtual ~MeshBundleVS() = default;

	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetIndices() const noexcept = 0;
};

class MeshBundleMS : public MeshBundle
{
public:
	virtual ~MeshBundleMS() = default;

	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetVertexIndices() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::uint32_t>& GetPrimIndices() const noexcept = 0;
};
#endif
