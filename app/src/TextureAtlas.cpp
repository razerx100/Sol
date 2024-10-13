#include <ranges>
#include <algorithm>
#include <cstring>
#include <optional>
#include <format>
#include <Exception.hpp>

#include <TextureAtlas.hpp>

TextureAtlas& TextureAtlas::AddTexture(
	const std::string& name, std::shared_ptr<void> texture, std::uint32_t width, std::uint32_t height
) noexcept {
	m_unprocessedData.emplace_back(TextureInfo{ name, width, height });
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

void TextureAtlas::CreateAtlas() noexcept
{
	// sort textures by their width and if width are same then by height
	auto predicate = [](const TextureInfo& var1, const TextureInfo& var2) noexcept
	{
		if (var1.width == var2.width)
			return var1.height > var2.height;
		else
			return var1.width > var2.width;
	};

	// Before texture info are sorted, remember their original indices to access the
	// corresponding texture data
	std::unordered_map<std::string, size_t> textureIndices{};
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index)
		textureIndices.insert_or_assign(m_unprocessedData[index].name, index);

	std::ranges::sort(m_unprocessedData, predicate);

	std::vector<UVU32> emptyPartitions{};
	std::vector<UVU32> processedData{};

	// Calculate coordinates
	// The width needs a border in each side, so multiplying with 2.
	const std::uint32_t widthBorderX2 = m_textureBorder * 2u;
	// Add the border of one of the sides to the height.
	m_texture.height += m_textureBorder;

	for (const TextureInfo& texData : m_unprocessedData)
	{
		bool newAllocationRequired = false;

		if (std::empty(emptyPartitions))
			newAllocationRequired = true;
		else
			newAllocationRequired = !ManagePartitions(
				emptyPartitions, texData, processedData, m_textureBorder
			);

		if (newAllocationRequired)
		{
			// The new height will already have the border added once, so just need it be added to the
			// other side.
			const std::uint32_t newHeight = m_texture.height + texData.height + m_textureBorder;

			// If the total width is longer than the current texture, then even after fitting it in
			// we might have some space left. So, creata an empty partition for that.
			if (m_texture.width > texData.width + widthBorderX2)
			{
				emptyPartitions.emplace_back(
					UVU32
					{
						.uStart = texData.width + widthBorderX2,
						.uEnd   = m_texture.width - m_textureBorder,
						.vStart = m_texture.height,
						.vEnd   = newHeight - m_textureBorder
					}
				);
			}

			processedData.emplace_back(
				UVU32
				{
					.uStart = m_textureBorder,
					.uEnd   = texData.width + m_textureBorder,
					.vStart = m_texture.height,
					.vEnd   = newHeight - m_textureBorder
				}
			);

			m_texture.height = newHeight;
		}

		if (m_texture.width < texData.width)
			m_texture.width = texData.width + widthBorderX2;
	}

	// Calculate offsets of textures and map them based on their names
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index)
	{
		const UVU32& texLocation   = processedData[index];
		const TextureInfo& texInfo = m_unprocessedData[index];
		const std::string& name    = texInfo.name;

		const UVInfo uvInfo{
			.uOffset = static_cast<float>(texLocation.uStart) / m_texture.width,
			.vOffset = static_cast<float>(texLocation.vStart) / m_texture.height,
			.uScale  = static_cast<float>(texInfo.width) / m_texture.width,
			.vScale  = static_cast<float>(texInfo.height) / m_texture.height
		};

		m_uvInfoMap.insert_or_assign(name, uvInfo);
	}

	const size_t bytesPerPixel = m_16bitsComponent ? 8u : 4u;
	const size_t rowPitch      = static_cast<size_t>(m_texture.width) * bytesPerPixel;
	const size_t textureSize   = rowPitch * m_texture.height;

	m_texture.data = std::make_shared<std::uint8_t[]>(textureSize);

	// Copy data
	for (size_t index = 0u; index < std::size(processedData); ++index)
	{
		const UVU32& currentCoord = processedData[index];
		const size_t textureIndex = textureIndices[m_unprocessedData[index].name];

		std::shared_ptr<void>& unprocessedTexture = m_unprocessedTextures[textureIndex];

		const size_t currentWidth    = static_cast<size_t>(currentCoord.uEnd) - currentCoord.uStart;
		const size_t currentRowPitch = currentWidth * bytesPerPixel;

		const size_t currentRowPitchStart = static_cast<size_t>(currentCoord.uStart) * bytesPerPixel;

		auto localTextureStart       = reinterpret_cast<std::uint8_t*>(m_texture.data.get());
		auto unprocessedTextureStart = reinterpret_cast<std::uint8_t*>(unprocessedTexture.get());

		for (size_t columnIndex =
			static_cast<size_t>(currentCoord.vStart), currentColumnIndex = 0u;
			columnIndex < currentCoord.vEnd; ++columnIndex, ++currentColumnIndex)
		{
			void* dst       = localTextureStart + currentRowPitchStart + (rowPitch * columnIndex);
			void const* src = unprocessedTextureStart + (currentRowPitch * currentColumnIndex);

			memcpy(dst, src, currentRowPitch);
		}
	}

	// Clean up
	m_unprocessedTextures = std::vector<std::shared_ptr<void>>{};
	m_unprocessedData     = std::vector<TextureInfo>{};
}

UVInfo TextureAtlas::GetUVInfo(const std::string& name) const noexcept
{
	auto data = m_uvInfoMap.find(name);

	if (data != std::end(m_uvInfoMap))
		return data->second;
	else {
		auto defaultColour = m_uvInfoMap.find("Default");

		if (defaultColour != std::end(m_uvInfoMap)) [[likely]]
			return defaultColour->second;
		else [[unlikely]]
			return {};
	}
}

bool TextureAtlas::ManagePartitions(
	std::vector<UVU32>& partitions, const TextureInfo& texData,
	std::vector<UVU32>& processedData, const std::uint32_t textureBorder
) noexcept {
	auto partition = std::ranges::find_if(
		partitions,
		[&texData](const UVU32& coord)
		{
			const std::uint32_t avilableWidth  = coord.uEnd - coord.uStart;
			const std::uint32_t avilableHeight = coord.vEnd - coord.vStart;

			return avilableWidth >= texData.width && avilableHeight >= texData.height;
		}
	);

	if (partition != std::end(partitions))
	{
		const UVU32& coord = *partition;

		const std::uint32_t newWidthStart  = coord.uStart + texData.width + textureBorder;
		const std::uint32_t newHeightStart = coord.vStart + texData.height + textureBorder;

		processedData.emplace_back(
			UVU32{
				.uStart = coord.uStart,
				.uEnd   = newWidthStart - textureBorder,
				.vStart = coord.vStart,
				.vEnd   = newHeightStart - textureBorder
			}
		);

		const UVU32 newHeightPartition = { newWidthStart, coord.uEnd, coord.vStart, coord.vEnd };
		const UVU32 newWidthPartition  = {
			coord.uStart, newWidthStart - textureBorder, newHeightStart, coord.vEnd
		};

		partitions.erase(partition);

		if (IsCoordSuitable(newHeightPartition))
			partitions.emplace_back(newHeightPartition);
		if (IsCoordSuitable(newWidthPartition))
			partitions.emplace_back(newWidthPartition);

		return true;
	}

	return false;
}
