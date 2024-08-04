#ifndef MESH_BOUNDS_HPP_
#define MESH_BOUNDS_HPP_
#include <MeshBundle.hpp>

namespace MeshBoundGenerator
{
	[[nodiscard]]
	std::vector<MeshBound> GetRectangle(const std::vector<Vertex>& vertices) noexcept;
};
#endif
