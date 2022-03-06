#ifndef __TEXTURE_ATLAS_HPP__
#define __TEXTURE_ATLAS_HPP__
#include <ITextureAtlas.hpp>
#include <unordered_map>

class ColourTexture {
public:
	void AddColour(const std::string& name, const Ceres::Float32_4& colour) noexcept;
	void CreateTexture() noexcept;

	size_t GetRowPitch() const noexcept;

	const std::vector<std::uint8_t>& GetTexture() const noexcept;
	const std::vector<std::string>& GetNames() const noexcept;

private:
	std::vector<std::string> m_colourNames;
	std::vector<Ceres::Float32_4> m_unprocessedColour;
	std::vector<std::uint8_t> m_texture;
};

class TextureAtlas : public ITextureAtlas {
public:
	void AddColour(
		const std::string& name, const Ceres::Float32_4& colour
	) noexcept override;
	void AddTexture(
		const std::string& name, const std::vector<std::uint8_t>& data,
		size_t rowPitch, size_t rows
	) noexcept override;
	void CreateAtlas() noexcept override;

	std::optional<UVF> GetUVData(const std::string& name) const noexcept override;

	size_t GetRowPitch() const noexcept override;
	size_t GetRows() const noexcept override;
	const std::vector<std::uint8_t>& GetTexture() const noexcept override;

private:
	struct TextureInfo {
		std::string name;
		size_t index;
		size_t rowPitch;
		size_t rows;
	};

	struct TextureData {
		std::vector<std::uint8_t> data;
		size_t rowPitch;
		size_t rows;
	};

	struct Partition {
		size_t index;
		UVU coord;
	};

	struct ProcessedData {
		std::string name;
		size_t textureIndex;
		UVU coord;
	};

private:
	float FixedValueToRelative(size_t value, size_t maxValue) const noexcept;
	bool PlaceTextureInProperPlace(
		const TextureInfo& texData,
		std::vector<Partition>& emptyPartitions,
		std::vector<ProcessedData>& processedData
	) const noexcept;

	void AddPartition(
		size_t index,
		size_t uStart, size_t uEnd, size_t vStart, size_t vEnd,
		std::vector<Partition>& emptyPartitions
	) const noexcept;

private:
	size_t m_rowPitch;
	size_t m_rows;

	std::vector<std::uint8_t> m_texture;
	std::unordered_map<std::string, UVF> m_uvDataMap;

	std::vector<TextureInfo> m_unprocessedData;
	std::vector<TextureData> m_unprocessedTextures;

	ColourTexture m_colourTextureManager;
};
#endif
