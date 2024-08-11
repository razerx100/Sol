#ifndef MODEL_SOL_HPP_
#define MODEL_SOL_HPP_
#include <cstdint>
#include <memory>
#include <vector>

#include <DirectXMath.h>

struct Meshlet
{
	std::uint32_t vertexCount;
	std::uint32_t vertexOffset;
	std::uint32_t primitiveCount;
	std::uint32_t primitiveOffset;
};

struct MeshDetailsVS
{
	std::uint32_t indexCount;
	std::uint32_t indexOffset;
};

struct UVInfo
{
	float uOffset = 0.f;
	float vOffset = 0.f;
	float uScale  = 1.f;
	float vScale  = 1.f;
};

struct MeshDetailsMS
{
	std::vector<Meshlet> meshlets;

	MeshDetailsMS() = default;
	MeshDetailsMS(const MeshDetailsMS& other) noexcept
		: meshlets{ other.meshlets }
	{}
	MeshDetailsMS& operator=(const MeshDetailsMS& other) noexcept
	{
		meshlets = other.meshlets;

		return *this;
	}

	MeshDetailsMS(MeshDetailsMS&& other) noexcept
		: meshlets{ std::move(other.meshlets) }
	{}
	MeshDetailsMS& operator=(MeshDetailsMS&& other) noexcept
	{
		meshlets = std::move(other.meshlets);

		return *this;
	}
};

class Model
{
public:
	virtual ~Model() = default;

	[[nodiscard]]
	virtual DirectX::XMMATRIX GetModelMatrix() const noexcept = 0;
	[[nodiscard]]
	virtual DirectX::XMFLOAT3 GetModelOffset() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetMaterialIndex() const noexcept = 0;

	[[nodiscard]]
	virtual std::uint32_t GetDiffuseIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetDiffuseUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetSpecularIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetSpecularUVInfo() const noexcept = 0;
};

class ModelVS : public Model
{
public:
	[[nodiscard]]
	virtual const MeshDetailsVS& GetMeshDetailsVS() const noexcept = 0;
};

class ModelMS : public Model
{
public:
	[[nodiscard]]
	// I am keeping this as non const, as I can potentially process the meshlets of multiple models
	// at once. So, I will need to keep them all in a single container, ie move from here.
	virtual MeshDetailsMS& GetMeshDetailsMS() noexcept = 0;
};
#endif
