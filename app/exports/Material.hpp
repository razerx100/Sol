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
	float             padding[3] = {}; // The struct needs to be 16bytes aligned. Yaaaay....
};

class Material
{
public:
	virtual ~Material() = default;

	[[nodiscard]]
	virtual MaterialData Get() const noexcept = 0;
};
#endif
