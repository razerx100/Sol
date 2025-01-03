#include <ranges>
#include <algorithm>
#include <cstring>
#include <optional>
#include <format>
#include <Exception.hpp>
#include <unordered_map>

#include <TextureAtlas.hpp>

TextureAtlas& TextureAtlas::AddTexture(
	const std::string& name, std::shared_ptr<void> texture, std::uint32_t width, std::uint32_t height
) noexcept {
	m_unprocessedData.emplace_back(
		TextureInfo
		{
			.name      = name,
			.width     = width,
			.height    = height,
			.dataIndex = std::size(m_unprocessedTextures)
		}
	);
	m_unprocessedTextures.emplace_back(std::move(texture));

	return *this;
}

TextureAtlas& TextureAtlas::AddTexture(const std::string& name, const std::string& fileName)
{
	auto texture = TextureTool::LoadTextureFromFile(fileName);

	if (texture)
		return AddTexture(name, std::move(texture.value()));
	else
		throw Exception{ "FileException", std::format("The texture file {} couldn't be found.", fileName) };
}

std::optional<size_t> TextureAtlas::FindSuitablePartition(
	const std::vector<UVU32>& freePartitions, const TextureInfo& texInfo
) noexcept {
	auto result = std::ranges::find_if(
		freePartitions,
		[&texInfo](const UVU32& coord)
		{
			const std::uint32_t avilableWidth  = coord.uEnd - coord.uStart;
			const std::uint32_t avilableHeight = coord.vEnd - coord.vStart;

			return avilableWidth >= texInfo.width && avilableHeight >= texInfo.height;
		}
	);

	std::optional<size_t> partitionIndex{};

	if (result != std::end(freePartitions))
		partitionIndex = std::distance(std::begin(freePartitions), result);

	return partitionIndex;
}

std::optional<TextureAtlas::PartitionDetails> TextureAtlas::FindOrCreateSuitablePartition(
	std::vector<UVU32>& freePartitions, const TextureInfo& texInfo
) const noexcept {
	std::optional<PartitionDetails> suitablePartition{};

	std::optional<size_t> oSuitableFreePartition = FindSuitablePartition(freePartitions, texInfo);

	const std::uint32_t currentWidth   = m_texture.width;
	const std::uint32_t currentHeight  = m_texture.height;

	if (oSuitableFreePartition)
	{
		auto suitableFreePartitionIt = std::next(
			std::begin(freePartitions), oSuitableFreePartition.value()
		);

		const Resolution currentResolution = Resolution{ .width = currentWidth, .height = currentHeight };

		PartitionDetails partitionDetails
		{
			.partition          = *suitableFreePartitionIt,
			.newAtlasResolution = currentResolution
		};

		suitablePartition = partitionDetails;

		freePartitions.erase(suitableFreePartitionIt);
	}
	else
	{
		// The goal would be to keep both the height and the width at a similar length.

		// One on each edge.
		const std::uint32_t borderX1 = m_textureBorder;
		const std::uint32_t borderX2 = borderX1 * 2u;

		const std::uint32_t currentTexHeightWithBorder = texInfo.height + borderX2;
		const std::uint32_t currentTexWidthWithBorder  = texInfo.width + borderX2;

		const std::uint32_t ifPlacedHorizontallyWidth  = currentWidth + currentTexWidthWithBorder;
		const std::uint32_t ifPlacedHorizontallyHeight = std::max(currentTexHeightWithBorder, currentHeight);

		const std::uint32_t horizontallyPlacedResolution
			= ifPlacedHorizontallyHeight * ifPlacedHorizontallyWidth;

		const std::uint32_t ifPlacedVerticallyWidth    = std::max(currentTexWidthWithBorder, currentWidth);
		const std::uint32_t ifPlacedVerticallyHeight   = currentHeight + currentTexHeightWithBorder;

		const std::uint32_t verticallyPlacedResolution
			= ifPlacedVerticallyHeight * ifPlacedVerticallyWidth;

		const bool horizontalDimensionCheck
			= ifPlacedHorizontallyWidth <= s_maxTextureDimensionLength
			&& ifPlacedHorizontallyHeight <= s_maxTextureDimensionLength;

		const bool verticalDimensionCheck
			= ifPlacedVerticallyWidth <= s_maxTextureDimensionLength
			&& ifPlacedVerticallyHeight <= s_maxTextureDimensionLength;

		// Place horizontally if horizontal res is smaller or if the vertical dimension limits
		// have been reached but the horizontal limits haven't be reached yet.
		const bool placeHorizontally
			= horizontallyPlacedResolution < verticallyPlacedResolution
			|| (horizontalDimensionCheck && !verticalDimensionCheck);

		// Since the vertical check is in an else if, don't need the resolution check.
		const bool placeVertically = verticalDimensionCheck;

		PartitionDetails partitionDetails{};

		if (placeHorizontally)
		{
			partitionDetails.partition = UVU32
			{
				.uStart = currentWidth + borderX1,
				.uEnd   = ifPlacedHorizontallyWidth - borderX1,
				.vStart = borderX1,
				.vEnd   = ifPlacedHorizontallyHeight - borderX1
			};
			partitionDetails.newAtlasResolution = Resolution
			{
				.width  = ifPlacedHorizontallyWidth,
				.height = ifPlacedHorizontallyHeight
			};

			suitablePartition = partitionDetails;
		}
		else if (placeVertically)
		{
			partitionDetails.partition = UVU32
			{
				.uStart = borderX1,
				.uEnd   = ifPlacedVerticallyWidth - borderX1,
				.vStart = currentHeight + borderX1,
				.vEnd   = ifPlacedVerticallyHeight - borderX1
			};
			partitionDetails.newAtlasResolution = Resolution
			{
				.width  = ifPlacedVerticallyWidth,
				.height = ifPlacedVerticallyHeight
			};

			suitablePartition = partitionDetails;
		}
	}

	return suitablePartition;
}

void TextureAtlas::PlaceTextureInSuitablePartition(
	std::vector<UVU32>& freePartitions, std::vector<UVU32>& processedPartitions,
	const TextureInfo& texInfo, const UVU32& suitablePartition, std::uint32_t textureBorder
) noexcept {
	const std::uint32_t newHorizontalPartitionStart
		= suitablePartition.uStart + texInfo.width + textureBorder;
	const std::uint32_t newVerticalPartitionStart
		= suitablePartition.vStart + texInfo.height + textureBorder;

	processedPartitions.emplace_back(
		UVU32
		{
			.uStart = suitablePartition.uStart,
			.uEnd   = newHorizontalPartitionStart - textureBorder,
			.vStart = suitablePartition.vStart,
			.vEnd   = newVerticalPartitionStart - textureBorder
		}
	);

	// Create the new free partitions.
	UVU32 newHorizontalPartition
	{
		.uStart = newHorizontalPartitionStart,
		.uEnd   = suitablePartition.uEnd,
		.vStart = suitablePartition.vStart,
		.vEnd   = newVerticalPartitionStart - textureBorder
	};

	UVU32 newVerticalPartition
	{
		.uStart = suitablePartition.uStart,
		.uEnd   = newHorizontalPartitionStart - textureBorder,
		.vStart = newVerticalPartitionStart,
		.vEnd   = suitablePartition.vEnd
	};

	auto GetArea = [](const UVU32& rect) noexcept -> std::uint32_t
		{
			const std::uint32_t width  = rect.uEnd - rect.uStart;
			const std::uint32_t height = rect.vEnd - rect.vStart;

			return width * height;
		};

	const std::uint32_t newVerticalArea   = GetArea(newVerticalPartition);
	const std::uint32_t newHorizontalArea = GetArea(newHorizontalPartition);

	// Add the diagonal partition to the partition which has the bigger area.
	if (newVerticalArea >= newHorizontalArea)
		newVerticalPartition.uEnd = suitablePartition.uEnd;
	else
		newHorizontalPartition.vEnd = suitablePartition.vEnd;

	// The greater than equal is necessary because if there was a single line, both the start and end
	// would be the same.
	auto IsPartitionValid = [](const UVU32& partition) noexcept -> bool
	{
		return partition.vEnd >= partition.vStart && partition.uEnd >= partition.uStart;
	};

	if (IsPartitionValid(newVerticalPartition))
		freePartitions.emplace_back(newVerticalPartition);

	if (IsPartitionValid(newHorizontalPartition))
		freePartitions.emplace_back(newHorizontalPartition);
}

void TextureAtlas::GenerateUVInfo(const std::vector<UVU32>& processedPartitions) noexcept
{
	// Calculate offsets of textures and map them based on their names
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index)
	{
		const UVU32& placedPartition = processedPartitions[index];
		const TextureInfo& texInfo   = m_unprocessedData[index];
		const std::string& name      = texInfo.name;

		const std::uint32_t atlasWidth  = m_texture.width;
		const std::uint32_t atlasHeight = m_texture.height;

		const UVInfo uvInfo
		{
			.uOffset = static_cast<float>(placedPartition.uStart) / atlasWidth,
			.vOffset = static_cast<float>(placedPartition.vStart) / atlasHeight,
			.uScale  = static_cast<float>(texInfo.width) / atlasWidth,
			.vScale  = static_cast<float>(texInfo.height) / atlasHeight
		};

		m_uvInfoData.emplace_back(UVData{ name, uvInfo });
	}
}

void TextureAtlas::AllocateAndCopyIntoAtlas(const std::vector<UVU32>& processedPartitions) noexcept
{
	const size_t bytesPerPixel = m_16bitsComponent ? 8u : 4u;
	const size_t atlasRowPitch = static_cast<size_t>(m_texture.width) * bytesPerPixel;
	const size_t textureSize   = atlasRowPitch * m_texture.height;

	m_texture.data         = std::make_shared<std::uint8_t[]>(textureSize);
	auto atlasTextureStart = reinterpret_cast<std::uint8_t*>(m_texture.data.get());

	// Copy data
	for (size_t index = 0u; index < std::size(processedPartitions); ++index)
	{
		const UVU32& localTexPartition = processedPartitions[index];
		const size_t textureIndex      = m_unprocessedData[index].dataIndex;

		std::shared_ptr<void>& localTexData = m_unprocessedTextures[textureIndex];

		const size_t localWidth
			= static_cast<size_t>(localTexPartition.uEnd) - localTexPartition.uStart;

		const size_t localTexRowPitch = localWidth * bytesPerPixel;

		const size_t localTexStartWithinAtlas
			= static_cast<size_t>(localTexPartition.uStart) * bytesPerPixel;

		auto localTexureStart = reinterpret_cast<std::uint8_t*>(localTexData.get());

		auto currentRowIndexWithinAtlas      = static_cast<size_t>(localTexPartition.vStart);
		const auto localTexRowEndWithinAtlas = static_cast<size_t>(localTexPartition.vEnd);

		size_t currentRowIndexWithinLocalTex = 0u;

		while (currentRowIndexWithinAtlas < localTexRowEndWithinAtlas)
		{
			const size_t currentRowStartWithinAtlas    = atlasRowPitch * currentRowIndexWithinAtlas;
			const size_t currentRowStartWithinLocalTex = localTexRowPitch * currentRowIndexWithinLocalTex;

			void* dst       = atlasTextureStart + localTexStartWithinAtlas + currentRowStartWithinAtlas;
			void const* src = localTexureStart + currentRowStartWithinLocalTex;

			memcpy(dst, src, localTexRowPitch);

			++currentRowIndexWithinAtlas;
			++currentRowIndexWithinLocalTex;
		}

		// Maybe should copy some kinda data as the border colour, instead of keeping the default values?
	}
}

void TextureAtlas::CreateAtlas()
{
	// sort textures by their width and if width are same then by height
	auto predicate = [](const TextureInfo& var1, const TextureInfo& var2) noexcept
	{
		if (var1.width == var2.width)
			return var1.height > var2.height;
		else
			return var1.width > var2.width;
	};

	std::ranges::sort(m_unprocessedData, predicate);

	std::vector<UVU32> freePartitions{};
	std::vector<UVU32> processedPartitions{};

	for (const TextureInfo& texInfo : m_unprocessedData)
	{
		std::optional<PartitionDetails> oPartitionDetails = FindOrCreateSuitablePartition(
			freePartitions, texInfo
		);

		// Stop placing new textures if the limit is reached. For now.
		assert(oPartitionDetails && "The dimension limit has been reached for this atlas.");

		const PartitionDetails partitionDetails = oPartitionDetails.value();

		const Resolution currentResolution{ .width = m_texture.width, .height = m_texture.height };

		// Increase the texture resolution if the partition is made on an extended region.
		if (partitionDetails.newAtlasResolution != currentResolution)
		{
			const Resolution newResolution = partitionDetails.newAtlasResolution;

			m_texture.height = newResolution.height;
			m_texture.width  = newResolution.width;
		}

		PlaceTextureInSuitablePartition(
			freePartitions, processedPartitions, texInfo, partitionDetails.partition,
			m_textureBorder
		);
	}

	GenerateUVInfo(processedPartitions);

	AllocateAndCopyIntoAtlas(processedPartitions);

	// Clean up
	m_unprocessedTextures = std::vector<std::shared_ptr<void>>{};
	m_unprocessedData     = std::vector<TextureInfo>{};
}

UVInfo TextureAtlas::GetUVInfo(const std::string& name) const noexcept
{
	auto data = std::ranges::find(
		m_uvInfoData, name, [](const UVData& uvData) { return uvData.name; }
	);

	if (data != std::end(m_uvInfoData))
		return data->uvInfo;
	else
		return UVInfo{};
}
