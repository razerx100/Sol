#ifndef TEXTURE_ATLAS_HPP_
#define TEXTURE_ATLAS_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

#include <DirectXMath.h>

template<typename T>
struct _UV {
	T uStart;
	T uEnd;
	T vStart;
	T vEnd;
};

typedef _UV<float> UVF;
typedef _UV<size_t> UVU;
typedef _UV<std::uint32_t> UVU32;

struct RGBA8 {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	std::uint8_t a;
};

enum class TextureFormat {
	Float32,
	UINT8
};

class ColourTexture {
public:
	void AddColour(const std::string& name, const DirectX::XMVECTORF32& colour) noexcept;
	void AddColour(
		const std::string& name, const RGBA8& colour
	) noexcept;
	void CreateTexture() noexcept;
	void SetPixelSizeInBytes(size_t pixelSizeInBytes) noexcept;

	size_t GetWidth() const noexcept;
	size_t GetPixelSizeInBytes() const noexcept;

	const std::vector<std::uint8_t>& GetTexture() const noexcept;
	const std::vector<std::string>& GetNames() const noexcept;

private:
	size_t m_pixelSizeInBytes;
	std::vector<std::string> m_colourNames;
	std::vector<DirectX::XMFLOAT4> m_unprocessedColourF32;
	std::vector<RGBA8> m_unprocessedColourU8;
	std::vector<std::uint8_t> m_texture;
};

class TextureAtlas {
public:
	void AddColour(
		const std::string& name, const DirectX::XMVECTORF32& colour
	) noexcept;
	void AddColour(
		const std::string& name, const RGBA8& colour
	) noexcept;

	void AddTexture(
		const std::string& name, const std::vector<std::uint8_t>& data,
		size_t width, size_t height
	) noexcept;

	void SetTextureFormat(TextureFormat format) noexcept;
	void CreateAtlas() noexcept;
	void CleanUpBuffer() noexcept;

	UVU32 GetPixelData(const std::string& name) const noexcept;

	std::uint32_t GetWidth() const noexcept;
	std::uint32_t GetHeight() const noexcept;
	size_t GetPixelSizeInBytes() const noexcept;

	const std::vector<std::uint8_t>& GetTexture() const noexcept;

private:
	struct TextureInfo {
		std::string name;
		size_t index;
		size_t height;
		size_t width;
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
	std::uint32_t m_width;
	std::uint32_t m_height;

	size_t m_pixelSizeInBytes;

	std::vector<std::uint8_t> m_texture;
	std::unordered_map<std::string, UVU32> m_pixelDataMap;

	std::vector<TextureInfo> m_unprocessedData;
	std::vector<TextureData> m_unprocessedTextures;

	ColourTexture m_colourTextureManager;
};
#endif
