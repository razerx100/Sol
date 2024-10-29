#ifndef BOUNDING_VOLUMES_HPP_
#define BOUNDING_VOLUMES_HPP_
#include <DirectXMath.h>

struct SphereBoundingVolume
{
	// The first three component has the centre and the W component has the radius.
	DirectX::XMFLOAT4 sphere;
};

// These need to be updated when the model is rotated. Need to add that functionality. So, it must be
// bounded to the model.
struct AxisAlignedBoundingBox
{
	DirectX::XMFLOAT3 maxAxes;
	DirectX::XMFLOAT3 minAxes;
};
#endif
