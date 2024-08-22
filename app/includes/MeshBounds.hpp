#ifndef MESH_BOUNDS_HPP_
#define MESH_BOUNDS_HPP_
#include <MeshBundle.hpp>

class RectangleBounds
{
public:
	[[nodiscard]]
	static std::vector<MeshBound> GetBounds(const std::vector<Vertex>& vertices) noexcept;
};
#endif
