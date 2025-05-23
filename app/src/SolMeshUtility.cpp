#include <SolMeshUtility.hpp>

namespace Sol
{
static MeshTextureDetails s_defaultTextureDetails{};

void CopyAndOffsetIndices(
	std::vector<std::uint32_t>& dst, const std::vector<std::uint32_t>& src, std::uint32_t offset
) noexcept {
	const size_t srcElementCount = std::size(src);
	const size_t dstElementCount = std::size(dst);

	const size_t dstDataStart    = dstElementCount;

	dst.resize(srcElementCount + dstElementCount);

	for (size_t index = 0u; index < srcElementCount; ++index)
		dst[index + dstDataStart] = offset + src[index];
}

void SetDefaultTextureDetails(std::uint32_t textureIndex, std::uint32_t bindingIndex) noexcept
{
	s_defaultTextureDetails = MeshTextureDetails
	{
		.textureIndex     = textureIndex,
		.textureBindIndex = bindingIndex
	};
}

const MeshTextureDetails& GetDefaultTextureDetails() noexcept
{
	return s_defaultTextureDetails;
}
}
