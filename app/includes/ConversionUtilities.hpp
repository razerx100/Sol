#ifndef CONVERSION_UTILITIES_HPP_
#define CONVERSION_UTILITIES_HPP_
#include <cstring>
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>

[[nodiscard]]
inline DirectX::XMMATRIX GetXMMatrix(const aiMatrix4x4& aiMatrix) noexcept
{
	static_assert(
		sizeof(ai_real) == sizeof(float), "ai real is a double and the copying will be wrong."
		);

	DirectX::XMFLOAT4X4 dxMatrixF{};

	memcpy(&dxMatrixF, &aiMatrix, sizeof(aiMatrix4x4));

	return DirectX::XMLoadFloat4x4(&dxMatrixF);
}

[[nodiscard]]
inline DirectX::XMFLOAT3 GetXMFloat3(const aiVector3D& aiVec3) noexcept
{
	static_assert(
		sizeof(ai_real) == sizeof(float), "ai real is a double and the copying will be wrong."
		);

	DirectX::XMFLOAT3 dxF3{};

	memcpy(&dxF3, &aiVec3, sizeof(aiVector3D));

	return dxF3;
}
#endif
