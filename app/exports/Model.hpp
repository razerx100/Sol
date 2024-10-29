#ifndef MODEL_SOL_HPP_
#define MODEL_SOL_HPP_
#include <cstdint>
#include <memory>
#include <vector>

#include <DirectXMath.h>

struct UVInfo
{
	float uOffset = 0.f;
	float vOffset = 0.f;
	float uScale  = 1.f;
	float vScale  = 1.f;
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
	virtual std::uint32_t GetMeshIndex() const noexcept = 0;

	[[nodiscard]]
	virtual std::uint32_t GetDiffuseIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetDiffuseUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetSpecularIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetSpecularUVInfo() const noexcept = 0;
};

class ModelBundle
{
public:
	virtual ~ModelBundle() = default;

	[[nodiscard]]
	virtual std::uint32_t GetMeshBundleIndex() const noexcept = 0;
	[[nodiscard]]
	virtual const std::vector<std::shared_ptr<Model>>& GetModels() const noexcept = 0;
};
#endif
