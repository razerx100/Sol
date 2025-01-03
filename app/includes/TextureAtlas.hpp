#ifndef TEXTURE_ATLAS_HPP_
#define TEXTURE_ATLAS_HPP_
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <concepts>
#include <limits>
#include <optional>
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

	void CreateAtlas();

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
		size_t        dataIndex;
	};

	struct UVData
	{
		std::string name;
		UVInfo      uvInfo;
	};

	struct Resolution
	{
		std::uint32_t width;
		std::uint32_t height;

		bool operator==(const Resolution& other) const noexcept
		{
			return other.width == width && other.height == height;
		}
	};

	struct PartitionDetails
	{
		UVU32      partition;
		Resolution newAtlasResolution;
	};

private:
	[[nodiscard]]
	// Returns the index or empty.
	static std::optional<size_t> FindSuitablePartition(
		const std::vector<UVU32>& freePartitions, const TextureInfo& texInfo
	) noexcept;

	[[nodiscard]]
	// Returns empty if can't find/create a partition without going over the dimension limits.
	std::optional<PartitionDetails> FindOrCreateSuitablePartition(
		std::vector<UVU32>& freePartitions, const TextureInfo& texInfo
	) const noexcept;

	static void PlaceTextureInSuitablePartition(
		std::vector<UVU32>& freePartitions, std::vector<UVU32>& processedPartitions,
		const TextureInfo& texInfo, const UVU32& suitablePartition, std::uint32_t textureBorder
	) noexcept;

	void GenerateUVInfo(const std::vector<UVU32>& processedPartitions) noexcept;
	void AllocateAndCopyIntoAtlas(const std::vector<UVU32>& processedPartitions) noexcept;

private:
	STexture                           m_texture;
	std::vector<UVData>                m_uvInfoData;
	std::vector<TextureInfo>           m_unprocessedData;
	std::vector<std::shared_ptr<void>> m_unprocessedTextures;
	std::uint32_t                      m_textureBorder;
	bool                               m_16bitsComponent;

	static constexpr std::uint16_t s_maxTextureDimensionLength = 16384u;

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
