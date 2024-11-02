#ifndef BOUNDING_VOLUMES_HPP_
#define BOUNDING_VOLUMES_HPP_
#include <DirectXMath.h>

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
#endif
