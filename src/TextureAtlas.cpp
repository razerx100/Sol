#include <ranges>
#include <algorithm>
#include <cstring>
#include <optional>

#include <TextureAtlas.hpp>

TextureAtlas::TextureAtlas() noexcept :
	m_width{ 0u }, m_height{ 0u }, m_16bitsComponent{ false }, m_atlasIndex{ 0u } {}

void TextureAtlas::AddTexture(
	const std::string& name, std::unique_ptr<std::uint8_t> texture,
	std::uint32_t width, std::uint32_t height
) noexcept {
	m_unprocessedData.emplace_back(name, width, height);
	m_unprocessedTextures.emplace_back(std::move(texture));
}

void TextureAtlas::SetIfComponentsAre16bits(bool component16bits) noexcept {
	m_16bitsComponent = component16bits;
}

void TextureAtlas::SetTextureIndex(size_t textureIndex) noexcept {
	m_atlasIndex = textureIndex;
}

void TextureAtlas::CreateAtlas() noexcept {
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
	const std::uint32_t textureBorder = 1u;
	const std::uint32_t borderX2 = textureBorder * 2u;
	m_height += textureBorder;

	for (const TextureInfo& texData : m_unprocessedData) {
		bool newAllocationRequired = false;
		if (std::empty(emptyPartitions))
			newAllocationRequired = true;
		else
			newAllocationRequired = !ManagePartitions(
				emptyPartitions, texData, processedData, textureBorder
			);

		if (newAllocationRequired) {
			std::uint32_t newHeight = m_height + texData.height + textureBorder;

			if (m_width > texData.width + borderX2) {
				emptyPartitions.emplace_back(
					UVU32{
						texData.width + 1u + borderX2, m_width - textureBorder,
						m_height + 1u, newHeight - textureBorder
					}
				);
			}

			processedData.emplace_back(
				1u + textureBorder, texData.width + textureBorder,
				m_height + 1u, newHeight - textureBorder
			);

			m_height = newHeight;
		}

		if (m_width < texData.width)
			m_width = texData.width + borderX2;
	}

	// Calculate offsets of textures and map them based on their names
	for (size_t index = 0u; index < std::size(m_unprocessedData); ++index) {
		const UVU32& texLocation = processedData[index];
		const TextureInfo& texInfo = m_unprocessedData[index];
		const std::string& name = texInfo.name;

		// A single pixel has the actual pixel data at a point, a point before it and a
		// point after it, 3 points in total. A texture with length n will have 2n + 1
		// points. A pixel coordinate from a texture represents the point of it's data. So,
		// Multiplying it with 2 should give us the actual position. But UV coordinate starts
		// from 0. So, we need to subtract 1 to get the coordinate of the point representing
		// the pixel data. But we want the coordinate of the point before the data. So, we
		// need to subtract 1 again, 2 in total.

		const float uRatio = static_cast<float>(texInfo.width) / m_width;
		const float vRatio = static_cast<float>(texInfo.height) / m_height;

		UVInfo uvInfo{
			CoordToUV(texLocation.uStart * 2u - 2u, m_width),
			CoordToUV(texLocation.vStart * 2u - 2u, m_height),
			uRatio, vRatio
		};

		m_uvInfoMap.insert_or_assign(name, uvInfo);
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

UVInfo TextureAtlas::GetUVInfo(const std::string& name) const noexcept {
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

bool TextureAtlas::ManagePartitions(
	std::vector<UVU32>& partitions, const TextureInfo& texData,
	std::vector<UVU32>& processedData, const std::uint32_t textureBorder
) const noexcept {
	for (auto it = std::begin(partitions); it != std::end(partitions); ++it) {
		UVU32& coord = (*it);
		std::uint32_t emptyWidth = coord.uEnd - coord.uStart + 1u;
		std::uint32_t emptyHeight = coord.vEnd - coord.vStart + 1u;

		if (emptyWidth >= texData.width && emptyHeight >= texData.height) {
			std::uint32_t newWidthStart = coord.uStart + texData.width + textureBorder;
			std::uint32_t newHeightStart = coord.vStart + texData.height + textureBorder;

			processedData.emplace_back(
				coord.uStart, newWidthStart - 1u - textureBorder,
				coord.vStart, newHeightStart - 1u - textureBorder
			);

			UVU32 fullHeight =
			{ newWidthStart, coord.uEnd, coord.vStart, coord.vEnd };
			UVU32 fullWidth =
			{ coord.uStart, newWidthStart - 1u - textureBorder, newHeightStart, coord.vEnd };

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

size_t TextureAtlas::GetTextureIndex() const noexcept {
	return m_atlasIndex;
}

bool TextureAtlas::DoesTextureExist() const noexcept {
	return m_width;
}
