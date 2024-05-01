#ifndef TEXTURE_TOOLS_HPP_
#define TEXTURE_TOOLS_HPP_
#include <cstdint>
#include <memory>
#include <string>
#include <optional>

struct STexture {
	inline STexture() noexcept : width{ 0u }, height{ 0u } {};

	STexture(const STexture&) = delete;
	STexture& operator=(const STexture&) = delete;

	inline STexture(STexture&& tex) noexcept
		: data{ std::move(tex.data) }, width{ tex.width }, height{ tex.height } {}

	inline STexture& operator=(STexture&& tex) noexcept {
		data = std::move(tex.data);
		width = tex.width;
		height = tex.height;
	}

	std::unique_ptr<std::uint8_t> data;
	std::uint32_t width;
	std::uint32_t height;
};

namespace TextureTool {
	[[nodiscard]]
	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept;

	void AddTextureToAtlas(const std::string& fileName, const std::string& texName) noexcept;
	void AddDefaultTexture() noexcept;
};
#endif
