#ifndef SCENE_MATERIAL_PROCESSOR_HPP_
#define SCENE_MATERIAL_PROCESSOR_HPP_
#include <memory>
#include <array>
#include <string>
#include <limits>
#include <optional>
#include <BlinnPhongLightTechnique.hpp>
#include <SceneProcessor.hpp>
#include <TextureAtlas.hpp>

namespace Sol
{
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

	template<class Renderer_t, class BlinnPhongLightTechnique_t>
	void LoadBlinnPhongMaterials(
		Renderer_t& renderer, BlinnPhongLightTechnique_t& blinnPhongTechnique
	) {
		const size_t materialCount = std::size(m_materialData);

		for (size_t index = 0u; index < materialCount; ++index)
			m_materialDetails[index].materialIndex = blinnPhongTechnique.AddMaterial(
				renderer, m_materialData[index]
			);
	}

	template<class Renderer_t>
	void LoadTexturesAsAtlas(Renderer_t& renderer)
	{
		TextureAtlas diffuseAtlas{};
		TextureAtlas specularAtlas{};

		// Maybe later I will make it so there are multiple atlas and only a certain number of
		// textures are put into a single atlas. As the precision would decrease the more textures
		// we put in one.
		for (const TexturePaths& texturePath : m_texturePaths)
			for (const TexturePath& diffuseTexturePath : texturePath.diffusePaths)
				diffuseAtlas.AddTexture(diffuseTexturePath.name, diffuseTexturePath.path);

		for (const TexturePaths& texturePath : m_texturePaths)
			for (const TexturePath& specularTexturePath : texturePath.specularPaths)
				specularAtlas.AddTexture(specularTexturePath.name, specularTexturePath.path);

		auto [diffuseTextureIndex, diffuseBindingIndex]
			= CreateAndAddAtlas(diffuseAtlas, renderer);
		auto [specularTextureIndex, specularBindingIndex]
			= CreateAndAddAtlas(specularAtlas, renderer);

		const size_t materialCount = std::size(m_materialDetails);

		for (size_t index = 0u; index < materialCount; ++index)
		{
			MaterialDetails& materialDetails         = m_materialDetails[index];
			const TexturePaths& materialTexturePaths = m_texturePaths[index];

			ConfigureMaterialTextures(
				materialDetails.diffuseDetails, materialTexturePaths.diffusePaths,
				diffuseAtlas, diffuseTextureIndex, diffuseBindingIndex
			);

			ConfigureMaterialTextures(
				materialDetails.specularDetails, materialTexturePaths.specularPaths,
				specularAtlas, specularTextureIndex, specularBindingIndex
			);
		}
	}

	template<class Renderer_t>
	void LoadTextures(Renderer_t& renderer)
	{
		const size_t materialCount = std::size(m_materialDetails);

		for (size_t index = 0u; index < materialCount; ++index)
		{
			MaterialDetails& materialDetails         = m_materialDetails[index];
			const TexturePaths& materialTexturePaths = m_texturePaths[index];

			ConfigureMaterialTextures(
				materialDetails.diffuseDetails, materialTexturePaths.diffusePaths, renderer
			);

			ConfigureMaterialTextures(
				materialDetails.specularDetails, materialTexturePaths.specularPaths, renderer
			);
		}
	}

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

	template<class Renderer_t>
	static void ConfigureMaterialTextures(
		std::vector<MeshTextureDetails>& textureDetails,
		const std::vector<TexturePath>& texturePaths, Renderer_t& renderer
	) noexcept {
		const size_t textureCount = std::size(texturePaths);

		for (size_t index = 0u; index < textureCount; ++index)
		{
			MeshTextureDetails& oneTextureDetails = textureDetails[index];
			const TexturePath& texturePath        = texturePaths[index];

			std::optional<STexture> texture = TextureTool::LoadTextureFromFile(texturePath.path);

			if (texture)
			{
				const auto textureIndex = static_cast<std::uint32_t>(
					renderer.AddTexture(std::move(*texture))
				);
				const auto textureBindIndex = static_cast<std::uint32_t>(
					renderer.BindTexture(textureIndex)
				);

				oneTextureDetails.textureIndex     = textureIndex;
				oneTextureDetails.textureBindIndex = textureBindIndex;

				// The UV info will be the default one for non atlas textures,
				// so no need to set it.
			}
		}
	}

	template<class Renderer_t>
	[[nodiscard]]
	static std::pair<std::uint32_t, std::uint32_t> CreateAndAddAtlas(
		class TextureAtlas& atlas, Renderer_t& renderer
	) {
		auto textureIndex = std::numeric_limits<std::uint32_t>::max();
		auto bindingIndex = std::numeric_limits<std::uint32_t>::max();

		if (atlas.DoUnprocessedTexturesExist())
		{
			atlas.CreateAtlas();

			textureIndex = static_cast<std::uint32_t>(
				renderer.AddTexture(std::move(atlas.MoveTexture()))
			);
			bindingIndex = static_cast<std::uint32_t>(
				renderer.BindTexture(textureIndex)
			);
		}

		return std::make_pair(textureIndex, bindingIndex);
	}

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
}
#endif
