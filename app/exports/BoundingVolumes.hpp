#ifndef BOUNDING_VOLUMES_HPP_
#define BOUNDING_VOLUMES_HPP_
#include <DirectXMath.h>
#include <utility>
#include <cstring>

struct SphereBoundingVolume
{
	// The first three component has the centre and the W component has the radius.
	DirectX::XMFLOAT4 sphere;
};

struct AxisAlignedBoundingBox
{
	DirectX::XMFLOAT4 maxAxes;
	DirectX::XMFLOAT4 minAxes;
};

struct ClusterNormalCone
{
	std::uint32_t packedCone; // xyz = axis, w = -cos(a + 90)

	void PackCone(const DirectX::XMFLOAT4& unpackedCone) noexcept
	{
		using namespace DirectX;

		auto PackComponent = [](float component) noexcept -> std::uint8_t
			{
				float compF = (component + 1.f) / 2.f;

				return static_cast<std::uint8_t>(compF * 255.f);
			};

		std::uint8_t packedConeU8[4] = { 0u, 0u, 0u, 0u };

		packedConeU8[0] = PackComponent(unpackedCone.x);
		packedConeU8[1] = PackComponent(unpackedCone.y);
		packedConeU8[2] = PackComponent(unpackedCone.z);
		packedConeU8[3] = static_cast<std::uint8_t>(unpackedCone.w * 255.f);

		memcpy(&packedCone, packedConeU8, sizeof(std::uint32_t));
	}

	[[nodiscard]]
	DirectX::XMFLOAT4 UnpackCone() const noexcept
	{
		using namespace DirectX;

		XMFLOAT4 unpackedCone
		{
			static_cast<float>((packedCone >> 0u)  && 255u),// x
			static_cast<float>((packedCone >> 8u)  && 255u),// y
			static_cast<float>((packedCone >> 16u) && 255u),// z
			static_cast<float>((packedCone >> 24u) && 255u) // w
		};

		unpackedCone.x /= 255.f;
		unpackedCone.y /= 255.f;
		unpackedCone.z /= 255.f;
		unpackedCone.w /= 255.f;

		// Since xyz are the axis, they could be in the negative direction.
		unpackedCone.x = unpackedCone.x * 2.f - 1.f;
		unpackedCone.y = unpackedCone.y * 2.f - 1.f;
		unpackedCone.z = unpackedCone.z * 2.f - 1.f;

		// But we don't care about the W parameter being negative I guess? As
		// we only want its length, and so want it to be positive.

		return unpackedCone;
	}
};
#endif
