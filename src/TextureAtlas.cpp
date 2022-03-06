#include <TextureAtlas.hpp>
#include <algorithm>
#include <cstring>
#include <optional>

// Colour Texture
void ColourTexture::AddColour(const std::string& name, const Ceres::Float32_4& colour) noexcept {
	m_colourNames.emplace_back(name);
	m_unprocessedColour.emplace_back(colour);
}

void ColourTexture::CreateTexture() noexcept {
	size_t textureSize = 16u * m_unprocessedColour.size();
	m_texture.resize(textureSize);

	for (size_t index = 0u; index < m_unprocessedColour.size(); ++index)
		std::memcpy(m_texture.data() + (index * 16u), &m_unprocessedColour[index], 16u);

	m_unprocessedColour = std::vector<Ceres::Float32_4>();
}

const std::vector<std::uint8_t>& ColourTexture::GetTexture() const noexcept {
	return m_texture;
}

const std::vector<std::string>& ColourTexture::GetNames() const noexcept {
	return m_colourNames;
}

size_t ColourTexture::GetRowPitch() const noexcept {
	return m_colourNames.size() * 16u;
}

// Texture Atlas
void TextureAtlas::AddColour(
	const std::string& name, const Ceres::Float32_4& colour
) noexcept {
	m_colourTextureManager.AddColour(name, colour);
}

void TextureAtlas::AddTexture(
	const std::string& name, const std::vector<std::uint8_t>& data,
	size_t rowPitch, size_t rows
) noexcept {
	m_unprocessedData.emplace_back(name, m_unprocessedData.size(), rowPitch, rows);
	m_unprocessedTextures.emplace_back(data, rowPitch, rows);
}

void TextureAtlas::CreateAtlas() noexcept {
	m_colourTextureManager.CreateTexture();

	AddTexture(
		"Colours", m_colourTextureManager.GetTexture(),
		m_colourTextureManager.GetRowPitch(), 1u
	);

	// Atlas Logic
	auto predicate = [](const TextureInfo& var1, const TextureInfo& var2) noexcept {
		if (var1.rowPitch == var2.rowPitch)
			return var1.rows > var2.rows;
		else
			return var1.rowPitch > var2.rowPitch;
	};

	std::sort(m_unprocessedData.begin(), m_unprocessedData.end(), predicate);

	std::vector<Partition> emptyPartitions;
	std::vector<ProcessedData> processedData;

	if (!m_unprocessedData.empty()) {
		TextureInfo& tex = m_unprocessedData.front();

		m_rowPitch = tex.rowPitch;
		m_rows += tex.rows;

		processedData.emplace_back(
			tex.name, tex.index, UVU{ 0u, tex.rowPitch - 1u, 0u, tex.rows - 1u }
		);
		m_unprocessedData.erase(m_unprocessedData.begin());
	}

	while (!m_unprocessedData.empty()) {
		TextureInfo& tex = m_unprocessedData.front();

		if (PlaceTextureInProperPlace(tex, emptyPartitions, processedData));
		else {
			size_t newRow = m_rows + tex.rows - 1u;
			processedData.emplace_back(
				tex.name, tex.index, UVU{ 0u, tex.rowPitch - 1u, m_rows, newRow }
			);

			if (tex.rowPitch < m_rowPitch)
				AddPartition(
					emptyPartitions.size(),
					tex.rowPitch, m_rowPitch - 1u, m_rows, newRow,
					emptyPartitions
				);

			m_rows += tex.rows;
		}

		m_unprocessedData.erase(m_unprocessedData.begin());
	}

	for (auto& element : processedData)
		m_uvDataMap.insert_or_assign(
			element.name,
			UVF{
				FixedValueToRelative(element.coord.uStart, m_rowPitch - 1u),
				FixedValueToRelative(element.coord.uEnd, m_rowPitch - 1u),
				FixedValueToRelative(element.coord.vStart, m_rows),
				FixedValueToRelative(element.coord.vEnd, m_rows)
			}
		);

	UVU colourData = {};
	for(auto& element : processedData)
		if (element.name == "Colours") {
			colourData = element.coord;

			break;
		}

	const std::vector<std::string>& colourNames = m_colourTextureManager.GetNames();

	for (size_t index = 0u; index < colourNames.size(); ++index)
		m_uvDataMap.insert_or_assign(
			colourNames[index],
			UVF{
				FixedValueToRelative(colourData.uStart + index * 16u, m_rowPitch - 1u),
				FixedValueToRelative(colourData.uStart + index * 16u + 15, m_rowPitch - 1u),
				FixedValueToRelative(colourData.vStart, m_rows),
				FixedValueToRelative(colourData.vStart, m_rows),
			}
		);

	m_texture.resize(m_rowPitch * m_rows);

	// Copy data
	for (size_t index = 0u; index < processedData.size(); ++index) {
		size_t textureIndex = processedData[index].textureIndex;

		for (size_t index1 = 0u; index1 < m_unprocessedTextures[textureIndex].rows; ++index1)
			std::memcpy(
				m_texture.data() + (m_unprocessedTextures[textureIndex].rowPitch * index)
				+ (m_rowPitch * index1),
				m_unprocessedTextures[textureIndex].data.data()
				+ (m_unprocessedTextures[textureIndex].rowPitch * index1),
				m_unprocessedTextures[textureIndex].rowPitch
			);
	}

	// Clean up
	m_unprocessedTextures = std::vector<TextureData>();
	m_unprocessedData = std::vector<TextureInfo>();
}

std::optional<UVF> TextureAtlas::GetUVData(const std::string& name) const noexcept {
	auto data = m_uvDataMap.find(name);

	if (data != m_uvDataMap.end())
		return data->second;
	else
		return {};
}

float TextureAtlas::FixedValueToRelative(size_t value, size_t maxValue) const noexcept {
	return static_cast<float>(value) / maxValue;
}

bool TextureAtlas::PlaceTextureInProperPlace(
	const TextureInfo& texData,
	std::vector<Partition>& emptyPartitions,
	std::vector<ProcessedData>& processedData
) const noexcept {
	std::optional<size_t> partitionIndex = {};
	for (size_t index = 0u; index < emptyPartitions.size(); ++index) {
		UVU ref = emptyPartitions[index].coord;
		if (ref.uEnd - ref.uStart + 1u >= texData.rowPitch
			&& ref.vEnd - ref.vStart + 1u >= texData.rows) {

			partitionIndex = index;
			break;
		}
	}

	if (partitionIndex) {
		size_t index = *partitionIndex;

		size_t newRowPitch = emptyPartitions[index].coord.uStart + texData.rowPitch - 1u;
		size_t newRows = emptyPartitions[index].coord.vStart + texData.rows - 1u;

		processedData.emplace_back(
			texData.name,
			texData.index,
			UVU{
				emptyPartitions[index].coord.uStart, newRowPitch,
				emptyPartitions[index].coord.vStart, newRows
			}
		);

		if (emptyPartitions[index].coord.uEnd > newRowPitch)
			AddPartition(
				emptyPartitions[index].index,
				newRowPitch + 1u, emptyPartitions[index].coord.uEnd,
				emptyPartitions[index].coord.vStart, newRows,
				emptyPartitions
			);

		emptyPartitions[index].coord.vStart = newRows + 1u;

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

size_t TextureAtlas::GetRowPitch() const noexcept {
	return m_rowPitch;
}

size_t TextureAtlas::GetRows() const noexcept {
	return m_rows;
}

const std::vector<std::uint8_t>& TextureAtlas::GetTexture() const noexcept {
	return m_texture;
}

