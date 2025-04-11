#ifndef SCENE_MATERIAL_PROCESSOR_HPP_
#define SCENE_MATERIAL_PROCESSOR_HPP_
#include <memory>
#include <array>
#include <string>
#include <limits>
#include <optional>
#include <BlinnPhongLightTechnique.hpp>
#include <SceneProcessor.hpp>
#include <Renderer.hpp>

class SceneMaterialProcessor
{
public:
	struct MaterialDetails
	{
		// Keeping the name, so the materials can be handled independently.
		std::string                     name{};
		// The indices from the renderer.
		std::uint32_t                   materialIndex = std::numeric_limits<std::uint32_t>::max();
		std::uint32_t                   pipelineIndex = std::numeric_limits<std::uint32_t>::max();
		std::vector<MeshTextureDetails> diffuseDetails{};
		std::vector<MeshTextureDetails> specularDetails{};
	};

private:
	struct TexturePath
	{
		std::string name;
		std::string path;
	};

	struct TexturePaths
	{
		std::vector<TexturePath> diffusePaths{};
		std::vector<TexturePath> specularPaths{};
	};

public:
	SceneMaterialProcessor(std::shared_ptr<SceneProcessor> scene)
		: m_scene{ std::move(scene) }, m_materialData{}, m_materialDetails{}, m_texturePaths{}
	{}

	void ProcessMeshAndMaterialData();

	void LoadBlinnPhongMaterials(BlinnPhongLightTechnique& blinnPhongTechnique);
	void LoadTexturesAsAtlas(Renderer& renderer);
	void LoadTextures(Renderer& renderer);

	void ClearTexturePaths() noexcept { m_texturePaths = std::vector<TexturePaths>{}; }

	[[nodiscard]]
	const MaterialDetails& GetMaterialDetails(size_t index) const noexcept
	{
		return m_materialDetails[index];
	}

	[[nodiscard]]
	const std::vector<MaterialDetails>& GetMaterialDetails() const noexcept
	{
		return m_materialDetails;
	}

private:
	static void ProcessMaterialTexture(
		aiTextureType textureType, aiMaterial const* material,
		std::vector<TexturePath>& texturePaths, const std::string& fileDirectory
	) noexcept;

	static void ConfigureMaterialTextures(
		std::vector<MeshTextureDetails>& textureDetails,
		const std::vector<TexturePath>& texturePaths, class TextureAtlas& atlas,
		std::uint32_t atlasIndex, std::uint32_t atlasBindIndex
	) noexcept;
	static void ConfigureMaterialTextures(
		std::vector<MeshTextureDetails>& textureDetails,
		const std::vector<TexturePath>& texturePaths, Renderer& renderer
	) noexcept;

	[[nodiscard]]
	static std::pair<std::uint32_t, std::uint32_t> CreateAndAddAtlas(
		class TextureAtlas& atlas, Renderer& renderer
	);

private:
	std::shared_ptr<SceneProcessor> m_scene;
	std::vector<BlinnPhongMaterial> m_materialData;
	std::vector<MaterialDetails>    m_materialDetails;
	std::vector<TexturePaths>       m_texturePaths;

public:
	SceneMaterialProcessor(const SceneMaterialProcessor&) = delete;
	SceneMaterialProcessor& operator=(const SceneMaterialProcessor&) = delete;

	SceneMaterialProcessor(SceneMaterialProcessor&& other) noexcept
		: m_scene{ std::move(other.m_scene) },
		m_materialData{ std::move(other.m_materialData) },
		m_materialDetails{ std::move(other.m_materialDetails) },
		m_texturePaths{ std::move(other.m_texturePaths) }
	{}
	SceneMaterialProcessor& operator=(SceneMaterialProcessor&& other) noexcept
	{
		m_scene           = std::move(other.m_scene);
		m_materialData    = std::move(other.m_materialData);
		m_materialDetails = std::move(other.m_materialDetails);
		m_texturePaths    = std::move(other.m_texturePaths);

		return *this;
	}
};
#endif
