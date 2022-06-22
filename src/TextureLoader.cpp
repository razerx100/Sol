#include <TextureLoader.hpp>
#include <array>
#include <SolThrowMacros.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TextureLoader {
	std::array<std::string, 11> extensions = {
		".jpg", ".jpeg", ".png", ".bmp", ".hdr", ".psd", ".tga",
		".gif", ".pic", ".pgm", ".ppm"
	};

	void ExtensionCheck(const std::string& fileName) {
		bool hasOneSupported = false;
		for (const std::string& extension : extensions)
			if (fileName.ends_with(extension)) {
				hasOneSupported = true;

				break;
			}

		if (!hasOneSupported)
			SOL_GENERIC_THROW("Unsupported texture format.");
	}

	STexture LoadTextureFromFile(const std::string& fileName) {
		ExtensionCheck(fileName);

		STexture texture;

		int width = 0;
		int height = 0;
		int componentCount = 0;

		stbi_info(fileName.c_str(), &width, &height, &componentCount);

		stbi_uc* data = stbi_load(fileName.c_str(), &width, &height, &componentCount, 4u);

		texture.data = std::unique_ptr<std::uint8_t>(reinterpret_cast<std::uint8_t*>(data));
		texture.width = static_cast<std::uint32_t>(width);
		texture.height = static_cast<std::uint32_t>(height);

		return texture;
	}
};
