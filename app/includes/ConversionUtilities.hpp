#ifndef CONVERSION_UTILITIES_HPP_
#define CONVERSION_UTILITIES_HPP_
#include <cstring>
#include <limits>
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>
#include <assimp/types.h>

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

[[nodiscard]]
inline DirectX::XMFLOAT4 GetXMFloat4(const aiColor3D& aiColour3) noexcept
{
	DirectX::XMFLOAT4 dxF4{};

	memcpy(&dxF4, &aiColour3, sizeof(aiColor3D));

	return dxF4;
}

[[nodiscard]]
inline DirectX::XMFLOAT4 GetXMFloat4(const aiColor4D& aiColour4) noexcept
{
	DirectX::XMFLOAT4 dxF4{};

	memcpy(&dxF4, &aiColour4, sizeof(aiColor4D));

	return dxF4;
}

[[nodiscard]]
inline DirectX::XMFLOAT3 NormaliseFloat3(const DirectX::XMFLOAT3& vec) noexcept
{
	using namespace DirectX;

	XMVECTOR vVec = XMLoadFloat3(&vec);

	vVec          = XMVector3Normalize(vVec);

	XMFLOAT3 nFloat3{};

	XMStoreFloat3(&nFloat3, vVec);

	return nFloat3;
}

[[nodiscard]]
inline bool isApproximatelyEqual(float a, float b)
{
	constexpr float epsilon = std::numeric_limits<float>::epsilon();

    return std::abs(a - b) <= std::max(std::abs(a), std::abs(b)) * epsilon;
}
#endif
