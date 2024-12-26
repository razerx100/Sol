#ifndef TEXTURE_ATLAS_HPP_
#define TEXTURE_ATLAS_HPP_
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <concepts>
#include <limits>
#include <TextureTools.hpp>

#include <DirectXMath.h>
#include <Model.hpp>

struct UVU32
{
	std::uint32_t uStart;
	std::uint32_t uEnd;
	std::uint32_t vStart;
	std::uint32_t vEnd;
};

class TextureAtlas
{
public:
	TextureAtlas()
		: m_texture{}, m_uvInfoData{}, m_unprocessedData{}, m_unprocessedTextures{},
		m_textureBorder{ 1u }, m_16bitsComponent{ false }
	{}

	TextureAtlas& AddTexture(
		const std::string& name, std::shared_ptr<void> texture,
		std::uint32_t width, std::uint32_t height
	) noexcept;
	TextureAtlas& AddTexture(const std::string& name, STexture&& texture) noexcept
	{
		return AddTexture(name, std::move(texture.data), texture.width, texture.height);
	}
	TextureAtlas& AddTexture(const std::string& name, const std::string& fileName);

	void SetIfComponentsAre16bits(bool component16bits) noexcept { m_16bitsComponent = component16bits; }
	void SetBorder(std::uint32_t border) noexcept { m_textureBorder = border; }

	void CreateAtlas() noexcept;

	[[nodiscard]]
	UVInfo GetUVInfo(const std::string& name) const noexcept;
	[[nodiscard]]
	std::uint32_t GetWidth() const noexcept { return m_texture.width; }
	[[nodiscard]]
	std::uint32_t GetHeight() const noexcept { return m_texture.height; }
	[[nodiscard]]
	bool IsTexture16bits() const noexcept { return m_16bitsComponent; }
	[[nodiscard]]
	bool DoesTextureExist() const noexcept { return m_texture.data != nullptr; }
	[[nodiscard]]
	bool DoUnprocessedTexturesExist() const noexcept { return !std::empty(m_unprocessedTextures); }
	[[nodiscard]]
	STexture MoveTexture() noexcept { return std::move(m_texture); }

private:
	struct TextureInfo
	{
		std::string   name;
		std::uint32_t width;
		std::uint32_t height;
	};

	struct UVData
	{
		std::string name;
		UVInfo      uvInfo;
	};

private:
	[[nodiscard]]
	static bool ManagePartitions(
		std::vector<UVU32>& partitions, const TextureInfo& texData,
		std::vector<UVU32>& processedData, const std::uint32_t textureBorder
	) noexcept;
	[[nodiscard]]
	static bool IsCoordSuitable(const UVU32& coord) noexcept
	{
		return coord.vEnd >= coord.vStart && coord.uEnd >= coord.uStart;
	}

private:
	STexture                           m_texture;
	std::vector<UVData>                m_uvInfoData;
	std::vector<TextureInfo>           m_unprocessedData;
	std::vector<std::shared_ptr<void>> m_unprocessedTextures;
	std::uint32_t                      m_textureBorder;
	bool                               m_16bitsComponent;

public:
	TextureAtlas(const TextureAtlas& other) noexcept = delete;
	TextureAtlas& operator=(const TextureAtlas& other) noexcept = delete;

	TextureAtlas(TextureAtlas&& other) noexcept
		: m_texture{ std::move(other.m_texture) }, m_uvInfoData{ std::move(other.m_uvInfoData) },
		m_unprocessedData{ std::move(other.m_unprocessedData) },
		m_unprocessedTextures{ std::move(other.m_unprocessedTextures) },
		m_textureBorder{ other.m_textureBorder }, m_16bitsComponent{ other.m_16bitsComponent }
	{}
	TextureAtlas& operator=(TextureAtlas&& other) noexcept
	{
		m_texture             = std::move(other.m_texture);
		m_uvInfoData          = std::move(other.m_uvInfoData);
		m_unprocessedData     = std::move(other.m_unprocessedData);
		m_unprocessedTextures = std::move(other.m_unprocessedTextures);
		m_textureBorder       = other.m_textureBorder;
		m_16bitsComponent     = other.m_16bitsComponent;

		return *this;
	}
};
#endif
