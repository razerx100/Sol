#include <ranges>
#include <algorithm>
#include <cstring>
#include <optional>

#include <TextureAtlas.hpp>

TextureAtlas::TextureAtlas() noexcept :
	m_width(0u), m_height(0u), m_16bitsComponent(false) {}

ColourTexture& TextureAtlas::GetColourTextureManager() noexcept {
	return m_colourTextureManager;
}

void TextureAtlas::AddTexture(
	const std::string& name, std::unique_ptr<std::uint8_t> texture,
	std::uint32_t width, std::uint32_t height
) noexcept {
	m_unprocessedData.emplace_back(name, width, height);
	m_unprocessedTextures.emplace_back(std::move(texture));
}

void TextureAtlas::CreateAtlas() noexcept {
	// Create and add colour texture
	m_colourTextureManager.CreateTexture();

	AddTexture(
		"Colours", m_colourTextureManager.MoveTexture(),
		m_colourTextureManager.GetWidth(), 1u
	);

	// sort textures by their width and if width are same then by height
	auto predicate = [](const TextureInfo& var1, const TextureInfo& var2) noexcept {
		if (var1.width == var2.width)
			return var1.height > var2.height;
		else
			return var1.width > var2.width;
	};

	// Before texture info are sorted, remember their original indices to access the
	// corresponding texture data
	std::unordered_map<std::string, size_t> textureIndices;
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index)
		textureIndices.insert_or_assign(m_unprocessedData[index].name, index);

	std::ranges::sort(m_unprocessedData, predicate);

	std::vector<UVU32> emptyPartitions;
	std::vector<UVU32> processedData;

	// Calculate coordinates
	for (const TextureInfo& texData : m_unprocessedData) {
		bool newAllocationRequired = false;
		if (std::empty(emptyPartitions))
			newAllocationRequired = true;
		else
			newAllocationRequired = !ManagePartitions(emptyPartitions, texData, processedData);

		if (newAllocationRequired) {
			std::uint32_t newHeight = m_height + texData.height;

			if (m_width > texData.width) {
				emptyPartitions.emplace_back(
					UVU32{ texData.width + 1u, m_width, m_height + 1u, newHeight }
				);
			}

			processedData.emplace_back(
				1u, texData.width,
				m_height + 1u, newHeight
			);

			m_height = newHeight;
		}

		if (m_width < texData.width)
			m_width = texData.width;
	}

	// set calculated coordinates based on their names
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index)
		m_pixelDataMap.insert_or_assign(m_unprocessedData[index].name, processedData[index]);

	// Find coordinates for Colours
	UVU32 colourData = {};
	if (auto result = m_pixelDataMap.find("Colours"); result != std::end(m_pixelDataMap))
		colourData = result->second;

	const std::vector<std::string>& colourNames = m_colourTextureManager.GetNames();

	// Add individual colours' coordinates into the pixel map
	for (size_t index = 0u; index < std::size(colourNames); ++index) {
		std::uint32_t uStart = colourData.uStart;
		std::uint32_t vStart = colourData.vStart;

		m_pixelDataMap.insert_or_assign(
			colourNames[index],
			UVU32{
				uStart + static_cast<std::uint32_t>(index),
				uStart + static_cast<std::uint32_t>(index),
				vStart,
				vStart
			}
		);
	}

	size_t bytesPerPixel = m_16bitsComponent ? 8u : 4u;
	size_t rowPitch = static_cast<size_t>(m_width) * bytesPerPixel;
	size_t textureSize = rowPitch * m_height;
	m_texture = std::unique_ptr<std::uint8_t>(new std::uint8_t[textureSize]);

	// Copy data
	for (size_t index = 0u; index < std::size(processedData); ++index) {
		UVU32& currentCoord = processedData[index];
		size_t textureIndex = textureIndices[m_unprocessedData[index].name];
		std::unique_ptr<std::uint8_t>& unprocessedTexture =
			m_unprocessedTextures[textureIndex];

		size_t currentRowPitch =
			(static_cast<size_t>(currentCoord.uEnd) - currentCoord.uStart + 1u)
			* bytesPerPixel;
		size_t currentRowPitchStart =
			(static_cast<size_t>(currentCoord.uStart) - 1u) * bytesPerPixel;

		for (size_t columnIndex =
			static_cast<size_t>(currentCoord.vStart) - 1u, currentColumnIndex = 0u;
			columnIndex < currentCoord.vEnd; ++columnIndex, ++currentColumnIndex) {

			memcpy(
				m_texture.get() + currentRowPitchStart + (rowPitch * columnIndex),
				unprocessedTexture.get() + (currentRowPitch * currentColumnIndex),
				currentRowPitch
			);
		}
	}

	// Clean up
	m_unprocessedTextures = std::vector<std::unique_ptr<std::uint8_t>>();
	m_unprocessedData = std::vector<TextureInfo>();
}

UVU32 TextureAtlas::GetPixelData(const std::string& name) const noexcept {
	auto data = m_pixelDataMap.find(name);

	if (data != std::end(m_pixelDataMap))
		return data->second;
	else
		return {};
}

std::uint32_t TextureAtlas::GetWidth() const noexcept {
	return m_width;
}

std::uint32_t TextureAtlas::GetHeight() const noexcept {
	return m_height;
}

std::unique_ptr<std::uint8_t> TextureAtlas::MoveTexture() noexcept {
	return std::move(m_texture);
}

bool TextureAtlas::IsTexture16bits() const noexcept {
	return m_16bitsComponent;
}

void TextureAtlas::SetIfComponentsAre16bits(bool component16bits) noexcept {
	m_16bitsComponent = component16bits;

	m_colourTextureManager.SetIfComponentsAre16bits(component16bits);
}

bool TextureAtlas::ManagePartitions(
	std::vector<UVU32>& partitions, const TextureInfo& texData,
	std::vector<UVU32>& processedData
) const noexcept {
	for (auto it = std::begin(partitions); it != std::end(partitions); ++it) {
		UVU32& coord = (*it);
		std::uint32_t emptyWidth = coord.uEnd - coord.uStart + 1u;
		std::uint32_t emptyHeight = coord.vEnd - coord.vStart + 1u;

		if (emptyWidth >= texData.width && emptyHeight >= texData.height) {
			std::uint32_t newWidthStart = coord.uStart + texData.width;
			std::uint32_t newHeightStart = coord.vStart + texData.height;

			processedData.emplace_back(
				coord.uStart, newWidthStart - 1u,
				coord.vStart, newHeightStart - 1u
			);

			UVU32 fullHeight =
			{ newWidthStart, coord.uEnd, coord.vStart, coord.vEnd };
			UVU32 fullWidth =
			{ coord.uStart, newWidthStart - 1u, newHeightStart, coord.vEnd };

			partitions.erase(it);

			if (IsCoordSuitable(fullHeight))
				partitions.emplace_back(fullHeight);
			if (IsCoordSuitable(fullWidth))
				partitions.emplace_back(fullWidth);

			return true;
		}
	}

	return false;
}

bool TextureAtlas::IsCoordSuitable(const UVU32& coord) const noexcept {
	return coord.vEnd >= coord.vStart && coord.uEnd >= coord.uStart;
}
