#ifndef SCENE_MATERIAL_PROCESSOR_HPP_
#define SCENE_MATERIAL_PROCESSOR_HPP_
#include <memory>
#include <string>
#include <limits>
#include <BlinnPhongLightTechnique.hpp>
#include <SceneProcessor.hpp>
#include <Renderer.hpp>

class SceneMaterialProcessor
{
public:
	struct TextureDetails
	{
		std::string   name{};
		// The indices of the resources.
		std::uint32_t textureIndex     = std::numeric_limits<std::uint32_t>::max();
		// The indices of the bound texures, which can be used.
		std::uint32_t textureBindIndex = std::numeric_limits<std::uint32_t>::max();
		// The indices from the material details container.
		std::uint32_t materialIndex    = std::numeric_limits<std::uint32_t>::max();
		UVInfo        uvInfo{};
	};

	struct MaterialDetails
	{
		// Keeping the name, so the materials can be handled independently.
		std::string   name{};
		// The indices from the renderer.
		std::uint32_t materialIndex = std::numeric_limits<std::uint32_t>::max();
		bool          transparent   = false;
	};

public:
	SceneMaterialProcessor(std::shared_ptr<SceneProcessor> scene)
		: m_scene{ std::move(scene) }, m_texturePaths{}, m_materialData{}, m_baseTextureDetails{},
		m_materialDetails{}, m_defaultTextureDetails{}
	{}

	void ProcessMeshAndMaterialData();

	void LoadBlinnPhongMaterials(BlinnPhongLightTechnique& blinnPhongTechnique);
	void LoadTexturesAsAtlas(Renderer& renderer);
	void LoadTextures(Renderer& renderer);

	[[nodiscard]]
	const MaterialDetails& GetMaterialDetails(size_t index) const noexcept
	{
		return m_materialDetails[index];
	}
	[[nodiscard]]
	const TextureDetails& GetBaseTextureDetails(const std::string& fileName) const noexcept;
	[[nodiscard]]
	const TextureDetails& GetBaseTextureDetails(size_t materialIndex) const noexcept;

	[[nodiscard]]
	const std::vector<TextureDetails>& GetBaseTextureDetails() const noexcept
	{
		return m_baseTextureDetails;
	}
	[[nodiscard]]
	const std::vector<MaterialDetails>& GetMaterialDetails() const noexcept
	{
		return m_materialDetails;
	}

private:
	struct TexturePath
	{
		std::string baseColour;
	};

private:
	std::shared_ptr<SceneProcessor> m_scene;
	std::vector<TexturePath>        m_texturePaths;
	std::vector<BlinnPhongMaterial> m_materialData;
	std::vector<TextureDetails>     m_baseTextureDetails;
	std::vector<MaterialDetails>    m_materialDetails;
	TextureDetails                  m_defaultTextureDetails;

public:
	SceneMaterialProcessor(const SceneMaterialProcessor&) = delete;
	SceneMaterialProcessor& operator=(const SceneMaterialProcessor&) = delete;

	SceneMaterialProcessor(SceneMaterialProcessor&& other) noexcept
		: m_scene{ std::move(other.m_scene) }, m_texturePaths{ std::move(other.m_texturePaths) },
		m_materialData{ std::move(other.m_materialData) },
		m_baseTextureDetails{ std::move(other.m_baseTextureDetails) },
		m_materialDetails{ std::move(other.m_materialDetails) },
		m_defaultTextureDetails{ std::move(other.m_defaultTextureDetails) }
	{}
	SceneMaterialProcessor& operator=(SceneMaterialProcessor&& other) noexcept
	{
		m_scene                 = std::move(other.m_scene);
		m_texturePaths          = std::move(other.m_texturePaths);
		m_materialData          = std::move(other.m_materialData);
		m_baseTextureDetails    = std::move(other.m_baseTextureDetails);
		m_materialDetails       = std::move(other.m_materialDetails);
		m_defaultTextureDetails = std::move(other.m_defaultTextureDetails);

		return *this;
	}
};
#endif
