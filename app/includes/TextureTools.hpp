#ifndef TEXTURE_TOOLS_HPP_
#define TEXTURE_TOOLS_HPP_
#include <cstdint>
#include <string>
#include <optional>
#include <Texture.hpp>

namespace Sol
{
namespace TextureTool
{
	[[nodiscard]]
	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept;
}
}
#endif
