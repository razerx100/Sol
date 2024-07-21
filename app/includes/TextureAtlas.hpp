#ifndef TEXTURE_ATLAS_HPP_
#define TEXTURE_ATLAS_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <memory>
#include <concepts>
#include <limits>
#include <TextureTools.hpp>

#include <DirectXMath.h>
#include <Model.hpp>

struct UVU32 {
	UVU32() = default;
	constexpr UVU32(
		std::uint32_t _uStart, std::uint32_t _uEnd,
		std::uint32_t _vStart, std::uint32_t _vEnd
	) noexcept : uStart(_uStart), uEnd(_uEnd), vStart(_vStart), vEnd(_vEnd) {}

	std::uint32_t uStart;
	std::uint32_t uEnd;
	std::uint32_t vStart;
	std::uint32_t vEnd;
};

class TextureAtlas {
public:
	TextureAtlas() noexcept;

	void AddTexture(
		const std::string& name, std::unique_ptr<std::uint8_t> texture,
		std::uint32_t width, std::uint32_t height
	) noexcept;
	void SetIfComponentsAre16bits(bool component16bits) noexcept;
	void SetTextureIndex(size_t textureIndex) noexcept;

	void CreateAtlas() noexcept;

	[[nodiscard]]
	UVInfo GetUVInfo(const std::string& name) const noexcept;
	[[nodiscard]]
	std::uint32_t GetWidth() const noexcept;
	[[nodiscard]]
	std::uint32_t GetHeight() const noexcept;
	[[nodiscard]]
	bool IsTexture16bits() const noexcept;
	[[nodiscard]]
	bool DoesTextureExist() const noexcept;
	[[nodiscard]]
	size_t GetTextureIndex() const noexcept;
	[[nodiscard]]
	std::unique_ptr<std::uint8_t> MoveTexture() noexcept;

private:
	struct TextureInfo {
		TextureInfo() = default;
		constexpr TextureInfo(
			const std::string& _name, std::uint32_t _width, std::uint32_t _height
		) noexcept : name(_name), width(_width), height(_height) {}

		std::string name;
		std::uint32_t width;
		std::uint32_t height;
	};

private:
	[[nodiscard]]
	bool ManagePartitions(
		std::vector<UVU32>& partitions, const TextureInfo& texData,
		std::vector<UVU32>& processedData, const std::uint32_t textureBorder
	) const noexcept;
	[[nodiscard]]
	bool IsCoordSuitable(const UVU32& coord) const noexcept;

	[[nodiscard]]
	constexpr float CoordToUV(std::uint32_t coord, std::uint32_t maxLength) const noexcept {
		return static_cast<float>(coord) / (maxLength * 2u);
	}

private:
	std::uint32_t m_width;
	std::uint32_t m_height;

	bool m_16bitsComponent;

	std::unique_ptr<std::uint8_t> m_texture;
	std::unordered_map<std::string, UVInfo> m_uvInfoMap;

	std::vector<TextureInfo> m_unprocessedData;
	std::vector<std::unique_ptr<std::uint8_t>> m_unprocessedTextures;

	size_t m_atlasIndex;
};
#endif
