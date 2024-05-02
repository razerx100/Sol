#ifndef MATERIAL_SOL_HPP_
#define MATERIAL_SOL_HPP_
#include <cstdint>

#include <DirectXMath.h>

struct MaterialData
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	float             shininess = 1.f;
};

struct UVInfo
{
	float uOffset;
	float vOffset;
	float uRatio;
	float vRatio;
};

class Material
{
public:
	virtual ~Material() = default;

	[[nodiscard]]
	virtual MaterialData Get() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetDiffuseIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetDiffuseUVInfo() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetSpecularIndex() const noexcept = 0;
	[[nodiscard]]
	virtual UVInfo GetSpecularUVInfo() const noexcept = 0;
};
#endif
