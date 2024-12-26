#ifndef SCENE_MATERIAL_PROCESSOR_HPP_
#define SCENE_MATERIAL_PROCESSOR_HPP_
#include <memory>
#include <string>
#include <limits>
#include <Material.hpp>
#include <SceneProcessor.hpp>
#include <Renderer.hpp>

class SceneMaterialProcessor
{
public:
	struct TextureDetails
	{
		std::string   baseName{};
		std::uint32_t materialIndex    = std::numeric_limits<std::uint32_t>::max();
		std::uint32_t baseTextureIndex = std::numeric_limits<std::uint32_t>::max();
		UVInfo        uvInfo{};
	};

public:
	SceneMaterialProcessor(std::shared_ptr<SceneProcessor> scene)
		: m_scene{ std::move(scene) }, m_texturePaths{}, m_materialData{}, m_textureDetails{}
	{}

	void ProcessMeshAndMaterialData();

	void LoadMaterials(Renderer& renderer);
	void LoadTexturesAsAtlas(Renderer& renderer);
	void LoadTextures(Renderer& renderer);

	[[nodiscard]]
	const std::vector<TextureDetails>& GetTextureDetails() const noexcept { return m_textureDetails; }
	[[nodiscard]]
	std::vector<TextureDetails>&& GetTextureDetails() noexcept { return std::move(m_textureDetails); }

private:
	struct TexturePath
	{
		std::string baseColour;
	};

private:
	std::shared_ptr<SceneProcessor> m_scene;
	std::vector<TexturePath>        m_texturePaths;
	std::vector<MaterialData>       m_materialData;
	std::vector<TextureDetails>     m_textureDetails;

public:
	SceneMaterialProcessor(const SceneMaterialProcessor&) = delete;
	SceneMaterialProcessor& operator=(const SceneMaterialProcessor&) = delete;

	SceneMaterialProcessor(SceneMaterialProcessor&& other) noexcept
		: m_scene{ std::move(other.m_scene) }, m_texturePaths{ std::move(other.m_texturePaths) },
		m_materialData{ std::move(other.m_materialData) },
		m_textureDetails{ std::move(other.m_textureDetails) }
	{}
	SceneMaterialProcessor& operator=(SceneMaterialProcessor&& other) noexcept
	{
		m_scene          = std::move(other.m_scene);
		m_texturePaths   = std::move(other.m_texturePaths);
		m_materialData   = std::move(other.m_materialData);
		m_textureDetails = std::move(other.m_textureDetails);

		return *this;
	}
};
#endif
