#include <TextureTools.hpp>
#include <array>
#include <SolThrowMacros.hpp>
#include <cassert>

#include <Sol.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TextureTool {
	std::array<std::string, 11> extensions = {
		".jpg", ".jpeg", ".png", ".bmp", ".hdr", ".psd", ".tga",
		".gif", ".pic", ".pgm", ".ppm"
	};

	[[nodiscard]]
	bool ExtensionCheck(const std::string& fileName) noexcept {
		bool hasOneSupported = false;
		for (const std::string& extension : extensions)
			if (fileName.ends_with(extension)) {
				hasOneSupported = true;

				break;
			}

		return hasOneSupported;
	}

	[[nodiscard]]
	std::optional<STexture> LoadTextureFromFile(const std::string& fileName) noexcept {
		const bool extensionSupport = ExtensionCheck(fileName);
		assert(extensionSupport && "Texture extention not supported.");

		if (!extensionSupport)
			return {};

		int width = 0;
		int height = 0;
		int componentCount = 0;

		stbi_info(fileName.c_str(), &width, &height, &componentCount);

		stbi_uc* data = stbi_load(fileName.c_str(), &width, &height, &componentCount, 4u);

		if (data != nullptr) {
			STexture texture;
			texture.data = std::unique_ptr<std::uint8_t>(reinterpret_cast<std::uint8_t*>(data));
			texture.width = static_cast<std::uint32_t>(width);
			texture.height = static_cast<std::uint32_t>(height);

			return texture;
		}
		else
			return {};
	}

	void AddTextureToAtlas(const std::string& fileName, const std::string& texName) noexcept {
		auto tex = LoadTextureFromFile(fileName);

		if (tex) {
			STexture& refTex = tex.value();

			Sol::textureAtlas->AddTexture(
				texName, std::move(refTex.data), refTex.width, refTex.height
			);
		}
	}

	void AddDefaultTexture() noexcept {
		std::array<std::uint8_t, 4u> defaultColour{ 255u, 255u, 255u, 255u };
		const size_t texSize = sizeof(std::uint8_t) * std::size(defaultColour);

		auto defaultTex = std::unique_ptr<std::uint8_t>(new std::uint8_t[texSize]);

		std::memcpy(defaultTex.get(), &defaultColour, texSize);

		// Should always be zero
		size_t defaultIndex = Sol::renderer->AddTexture(std::move(defaultTex), 1u, 1u);
	}
};
