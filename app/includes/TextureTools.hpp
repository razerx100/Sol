#ifndef TEXTURE_TOOLS_HPP_
#define TEXTURE_TOOLS_HPP_
#include <cstdint>
#include <memory>
#include <string>
#include <optional>

struct STexture
{
	STexture() : data{ nullptr }, width{0u}, height{0u} {};

	std::unique_ptr<std::uint8_t> data;
	std::uint32_t                 width;
	std::uint32_t                 height;

	STexture(const STexture&) = delete;
	STexture& operator=(const STexture&) = delete;

	STexture(STexture&& tex) noexcept
		: data{ std::move(tex.data) }, width{ tex.width }, height{ tex.height }
	{}

	STexture& operator=(STexture&& tex) noexcept
	{
		data   = std::move(tex.data);
		width  = tex.width;
		height = tex.height;

		return *this;
	}
};

namespace TextureTool
{
	[[nodiscard]]
	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept;

	//void AddTextureToAtlas(const std::string& fileName, const std::string& texName) noexcept;
	//void AddDefaultTexture() noexcept;
};
#endif
