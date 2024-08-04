#include <TextureTools.hpp>
#include <array>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TextureTool
{
	[[nodiscard]]
	static bool ExtensionCheck(const std::string& fileName) noexcept
	{
		static std::array<std::string, 11> extensions =
		{
			".jpg", ".jpeg", ".png", ".bmp", ".hdr", ".psd", ".tga",
			".gif", ".pic", ".pgm", ".ppm"
		};

		bool hasOneSupported = false;
		for (const std::string& extension : extensions)
			if (fileName.ends_with(extension))
			{
				hasOneSupported = true;

				break;
			}

		return hasOneSupported;
	}

	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept
	{
		const bool extensionSupport = ExtensionCheck(fileName);
		assert(extensionSupport && "Texture extention not supported.");

		if (!extensionSupport)
			return {};

		int width          = 0;
		int height         = 0;
		int componentCount = 0;

		stbi_info(fileName.c_str(), &width, &height, &componentCount);

		stbi_uc* data = stbi_load(fileName.c_str(), &width, &height, &componentCount, 4u);

		if (data != nullptr)
		{
			STexture texture{};
			texture.data   = std::unique_ptr<std::uint8_t>(reinterpret_cast<std::uint8_t*>(data));
			texture.width  = static_cast<std::uint32_t>(width);
			texture.height = static_cast<std::uint32_t>(height);

			return texture;
		}
		else
			return {};
	}
};
