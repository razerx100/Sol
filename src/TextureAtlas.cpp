#include <algorithm>
#include <cstring>
#include <optional>

#include <TextureAtlas.hpp>

// Colour Texture
void ColourTexture::AddColour(
	const std::string& name, const DirectX::XMVECTORF32& colour
) noexcept {
	m_colourNames.emplace_back(name);

	DirectX::XMFLOAT4 colourF4 = {};
	DirectX::XMStoreFloat4(&colourF4, colour);

	if (m_pixelSizeInBytes == 16u)
		m_unprocessedColourF32.emplace_back(std::move(colourF4));
	else {
		RGBA8 colourU8 = {};
		colourU8.r = static_cast<std::uint8_t>(255u * colourF4.x);
		colourU8.g = static_cast<std::uint8_t>(255u * colourF4.y);
		colourU8.b = static_cast<std::uint8_t>(255u * colourF4.z);
		colourU8.a = static_cast<std::uint8_t>(255u * colourF4.w);

		m_unprocessedColourU8.emplace_back(std::move(colourU8));
	}
}

void ColourTexture::AddColour(const std::string& name, const RGBA8& colour) noexcept {
	m_colourNames.emplace_back(name);

	if (m_pixelSizeInBytes == 16u) {
		DirectX::XMFLOAT4 colourF32 = {};
		colourF32.x = static_cast<float>(colour.r) / 255u;
		colourF32.y = static_cast<float>(colour.g) / 255u;
		colourF32.z = static_cast<float>(colour.b) / 255u;
		colourF32.w = static_cast<float>(colour.a) / 255u;

		m_unprocessedColourF32.emplace_back(std::move(colourF32));
	}
	else
		m_unprocessedColourU8.emplace_back(colour);
}

void ColourTexture::CreateTexture() noexcept {
	size_t textureSize = m_pixelSizeInBytes *
		(m_unprocessedColourF32.size() + m_unprocessedColourU8.size());
	m_texture.resize(textureSize);

	if (m_pixelSizeInBytes == 16u) {
		for (size_t index = 0u; index < m_unprocessedColourF32.size(); ++index)
			std::memcpy(
				m_texture.data() + (index * m_pixelSizeInBytes),
				&m_unprocessedColourF32[index], m_pixelSizeInBytes
			);

		m_unprocessedColourF32 = std::vector<DirectX::XMFLOAT4>();
	}
	else {
		for (size_t index = 0u; index < m_unprocessedColourU8.size(); ++index)
			std::memcpy(
				m_texture.data() + (index * m_pixelSizeInBytes),
				&m_unprocessedColourU8[index], m_pixelSizeInBytes
			);

		m_unprocessedColourU8 = std::vector<RGBA8>();
	}
}

void ColourTexture::SetPixelSizeInBytes(size_t pixelSizeInBytes) noexcept {
	m_pixelSizeInBytes = pixelSizeInBytes;
}

const std::vector<std::uint8_t>& ColourTexture::GetTexture() const noexcept {
	return m_texture;
}

const std::vector<std::string>& ColourTexture::GetNames() const noexcept {
	return m_colourNames;
}

size_t ColourTexture::GetWidth() const noexcept {
	return m_colourNames.size();
}

size_t ColourTexture::GetPixelSizeInBytes() const noexcept {
	return m_pixelSizeInBytes;
}

// Texture Atlas
void TextureAtlas::AddColour(
	const std::string& name, const DirectX::XMVECTORF32& colour
) noexcept {
	m_colourTextureManager.AddColour(name, colour);
}

void TextureAtlas::AddColour(const std::string& name, const RGBA8& colour) noexcept {
	m_colourTextureManager.AddColour(name, colour);
}

void TextureAtlas::AddTexture(
	const std::string& name, const std::vector<std::uint8_t>& data,
	size_t width, size_t height
) noexcept {
	size_t rowPitch = m_pixelSizeInBytes * width;

	m_unprocessedData.emplace_back(
		name, m_unprocessedData.size(), height, width
	);
	m_unprocessedTextures.emplace_back(data, rowPitch, height);
}

void TextureAtlas::CreateAtlas() noexcept {
	m_colourTextureManager.CreateTexture();

	AddTexture(
		"Colours", m_colourTextureManager.GetTexture(),
		m_colourTextureManager.GetWidth(), 1u
	);

	// Atlas Logic
	auto predicate = [](const TextureInfo& var1, const TextureInfo& var2) noexcept {
		if (var1.width == var2.width)
			return var1.height > var2.height;
		else
			return var1.width > var2.width;
	};

	std::sort(m_unprocessedData.begin(), m_unprocessedData.end(), predicate);

	std::vector<Partition> emptyPartitions;
	std::vector<ProcessedData> processedData;

	if (!m_unprocessedData.empty()) {
		TextureInfo& tex = m_unprocessedData.front();

		m_width = static_cast<std::uint32_t>(tex.width);
		m_height += static_cast<std::uint32_t>(tex.height);

		processedData.emplace_back(
			tex.name, tex.index, UVU{ 0u, tex.width, 0u, tex.height }
		);
		m_unprocessedData.erase(m_unprocessedData.begin());
	}

	while (!m_unprocessedData.empty()) {
		TextureInfo& tex = m_unprocessedData.front();

		if (PlaceTextureInProperPlace(tex, emptyPartitions, processedData));
		else {
			size_t newHeight = m_height + tex.height;
			processedData.emplace_back(
				tex.name, tex.index, UVU{ 0u, tex.width, m_height + 1u, newHeight }
			);

			if (tex.width < m_width)
				AddPartition(
					emptyPartitions.size(),
					tex.width, m_width, m_height, newHeight,
					emptyPartitions
				);

			m_height += static_cast<std::uint32_t>(tex.height);
		}

		m_unprocessedData.erase(m_unprocessedData.begin());
	}

	// Till here

	for (auto& element : processedData) {
		UVU& coord = element.coord;

		m_pixelDataMap.insert_or_assign(
			element.name,
			UVU32{
				static_cast<std::uint32_t>(coord.uStart + 1u),
				static_cast<std::uint32_t>(coord.uEnd),
				static_cast<std::uint32_t>(coord.vStart + 1u),
				static_cast<std::uint32_t>(coord.vEnd)
			}
		);
	}

	UVU32 colourData = {};
	if (auto result = m_pixelDataMap.find("Colours"); result != m_pixelDataMap.end())
		colourData = result->second;

	const std::vector<std::string>& colourNames = m_colourTextureManager.GetNames();

	for (size_t index = 0u; index < colourNames.size(); ++index) {
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

	size_t rowPitch = m_width * m_pixelSizeInBytes;
	m_texture.resize(rowPitch * m_height);

	// Copy data
	for (size_t index = 0u; index < processedData.size(); ++index) {
		size_t textureIndex = processedData[index].textureIndex;

		TextureData& unprocessedTextureData = m_unprocessedTextures[textureIndex];
		for (size_t index1 = 0u; index1 < unprocessedTextureData.rows; ++index1)
			std::memcpy(
				m_texture.data() + (unprocessedTextureData.rowPitch * index)
				+ (rowPitch * index1),
				unprocessedTextureData.data.data()
				+ (unprocessedTextureData.rowPitch * index1),
				unprocessedTextureData.rowPitch
			);
	}

	// Clean up
	m_unprocessedTextures = std::vector<TextureData>();
	m_unprocessedData = std::vector<TextureInfo>();
}

UVU32 TextureAtlas::GetPixelData(const std::string& name) const noexcept {
	auto data = m_pixelDataMap.find(name);

	if (data != m_pixelDataMap.end())
		return data->second;
	else
		return {};
}

bool TextureAtlas::PlaceTextureInProperPlace(
	const TextureInfo& texData,
	std::vector<Partition>& emptyPartitions,
	std::vector<ProcessedData>& processedData
) const noexcept {
	std::optional<size_t> partitionIndex = {};
	for (size_t index = 0u; index < emptyPartitions.size(); ++index) {
		const UVU& ref = emptyPartitions[index].coord;
		if (ref.uEnd - ref.uStart>= texData.width
			&& ref.vEnd - ref.vStart>= texData.height) {

			partitionIndex = index;
			break;
		}
	}

	if (partitionIndex) {
		size_t index = *partitionIndex;

		Partition& emptyPartition = emptyPartitions[index];
		UVU& coord = emptyPartition.coord;

		size_t newWidth = coord.uStart + texData.width;
		size_t newRows = coord.vStart + texData.height;

		processedData.emplace_back(
			texData.name,
			texData.index,
			UVU{
				coord.uStart + 1u, newWidth,
				coord.vStart + 1u, newRows
			}
		);

		if (coord.uEnd > newWidth)
			AddPartition(
				emptyPartition.index,
				newWidth, coord.uEnd,
				coord.vStart, newRows,
				emptyPartitions
			);

		coord.vStart = newRows;

		return true;
	}
	else
		return false;
}

void TextureAtlas::AddPartition(
	size_t index,
	size_t uStart, size_t uEnd, size_t vStart, size_t vEnd,
	std::vector<Partition>& emptyPartitions
) const noexcept {
	emptyPartitions.emplace_back(index, UVU{ uStart, uEnd, vStart, vEnd });

	std::sort(
		emptyPartitions.begin(), emptyPartitions.end(),
		[](const Partition& part1, const Partition& part2) {
			return part1.index < part2.index;
		}
	);
}

void TextureAtlas::SetTextureFormat(TextureFormat format) noexcept {
	size_t pixelSizeInBytes = 0u;

	if (TextureFormat::Float32 == format)
		pixelSizeInBytes = 16u;
	else if (TextureFormat::UINT8 == format)
		pixelSizeInBytes = 4u;

	m_pixelSizeInBytes = pixelSizeInBytes;

	m_colourTextureManager.SetPixelSizeInBytes(pixelSizeInBytes);
}

std::uint32_t TextureAtlas::GetWidth() const noexcept {
	return m_width;
}

std::uint32_t TextureAtlas::GetHeight() const noexcept {
	return m_height;
}

size_t TextureAtlas::GetPixelSizeInBytes() const noexcept {
	return m_pixelSizeInBytes;
}

const std::vector<std::uint8_t>& TextureAtlas::GetTexture() const noexcept {
	return m_texture;
}

void TextureAtlas::CleanUpBuffer() noexcept {
	m_texture = std::vector<std::uint8_t>();
	// Should clean the texture too once it gets big
}
