#include <ranges>
#include <algorithm>
#include <MeshBounds.hpp>

std::vector<MeshBound> RectangleBounds::GetBounds(const std::vector<Vertex>& vertices) noexcept
{
	DirectX::XMFLOAT3 pAxes{};
	DirectX::XMFLOAT3 nAxes{};

	for (const auto& vertex : vertices)
	{
		pAxes.x = std::max(vertex.position.x, pAxes.x);
		pAxes.y = std::max(vertex.position.y, pAxes.y);
		pAxes.z = std::max(vertex.position.z, pAxes.z);

		nAxes.x = std::min(vertex.position.x, nAxes.x);
		nAxes.y = std::min(vertex.position.y, nAxes.y);
		nAxes.z = std::min(vertex.position.z, nAxes.z);
	}

	std::vector<MeshBound> meshBounds
	{
		MeshBound{ DirectX::XMFLOAT3{ nAxes.x, pAxes.y, nAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ pAxes.x, pAxes.y, nAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ pAxes.x, nAxes.y, nAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ nAxes.x, nAxes.y, nAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ nAxes.x, pAxes.y, pAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ pAxes.x, pAxes.y, pAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ pAxes.x, nAxes.y, pAxes.z } },
		MeshBound{ DirectX::XMFLOAT3{ nAxes.x, nAxes.y, pAxes.z } }
	};

	return meshBounds;
}
