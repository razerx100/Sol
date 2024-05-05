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

struct MeshDetailsMS
{
	std::vector<Meshlet> meshlets;
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
	virtual bool IsLightSource() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetMeshIndex() const noexcept = 0;
};

class ModelVS : public virtual Model
{
public:
	[[nodiscard]]
	virtual const MeshDetailsVS& GetMeshDetailsVS() const noexcept = 0;
};

class ModelMS : public virtual Model
{
public:
	[[nodiscard]]
	virtual MeshDetailsMS&& GetMeshDetailsMS() noexcept = 0;
};
#endif
