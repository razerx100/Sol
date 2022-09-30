#ifndef TEXTURE_LOADER_HPP_
#define TEXTURE_LOADER_HPP_
#include <cstdint>
#include <memory>
#include <string>
#include <optional>

struct STexture {
	STexture() = default;

	STexture(const STexture&) = delete;
	STexture& operator=(const STexture&) = delete;

	inline STexture(STexture&& tex) noexcept
		: data(std::move(tex.data)), width(tex.width), height(tex.height) {}

	inline STexture& operator=(STexture&& tex) noexcept {
		data = std::move(tex.data);
		width = tex.width;
		height = tex.height;
	}

	std::unique_ptr<std::uint8_t> data;
	std::uint32_t width = 0u;
	std::uint32_t height = 0u;
};

namespace TextureLoader {
	[[nodiscard]]
	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept;

	void AddTextureToAtlas(const std::string& fileName, const std::string& texName) noexcept;
};
#endif
