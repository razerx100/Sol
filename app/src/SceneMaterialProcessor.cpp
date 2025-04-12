#include <SceneMaterialProcessor.hpp>
#include <ConversionUtilities.hpp>
#include <TextureAtlas.hpp>
#include <array>
#include <ranges>
#include <algorithm>
#include <assimp/GltfMaterial.h>

// Scene Material Processor
void SceneMaterialProcessor::ProcessMeshAndMaterialData()
{
	aiScene const* scene = m_scene->GetScene();

	const size_t materialCount = scene->mNumMaterials;
	aiMaterial** materials     = scene->mMaterials;

	const std::string fileDirectory = m_scene->GetFileDirectory();

	m_materialData.reserve(materialCount);
	m_materialDetails.reserve(materialCount);

	for (size_t index = 0u; index < materialCount; ++index)
	{
		aiMaterial* material = materials[index];

		// Material
		aiColor4D diffuse{ 1.f, 1.f, 1.f, 1.f };
		aiColor4D specular{ 1.f, 1.f, 1.f, 1.f };
		aiColor4D ambient{ 1.f, 1.f, 1.f, 1.f };
		float shininess = 1.f;
		float opacity   = 1.f;
		aiString name{};

		material->Get(AI_MATKEY_NAME, name);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);
		material->Get(AI_MATKEY_OPACITY, opacity);

		aiString alphaMode{};

		material->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode);

		const bool isTransparent =
			!isApproximatelyEqual(opacity, 1.f) || alphaMode != aiString{ "OPAQUE" };

		m_materialData.emplace_back(
			BlinnPhongMaterial
			{
				.ambient   = GetXMFloat4(ambient),
				.diffuse   = GetXMFloat4(diffuse),
				.specular  = GetXMFloat4(specular),
				.shininess = shininess
			}
		);

		using ShaderType = PSOStorage::ShaderType;

		MaterialDetails materialDetails
		{
			.name          = name.C_Str(),
			.materialIndex = static_cast<std::uint32_t>(index),
			.pipelineIndex = PSOStorage::GetPipelineIndex(
				isTransparent ? ShaderType::TransparentLight : ShaderType::OpaqueLight
			)
		};

		TexturePaths texturePaths{};

		ProcessMaterialTexture(
			aiTextureType_DIFFUSE, material, texturePaths.diffusePaths, fileDirectory
		);
		ProcessMaterialTexture(
			aiTextureType_SPECULAR, material, texturePaths.specularPaths, fileDirectory
		);

		m_materialDetails.emplace_back(std::move(materialDetails));
		m_texturePaths.emplace_back(std::move(texturePaths));
	}
}

void SceneMaterialProcessor::ProcessMaterialTexture(
	aiTextureType textureType, aiMaterial const* material,
	std::vector<TexturePath>& texturePaths, const std::string& fileDirectory
) noexcept {
	const std::uint32_t textureCount = material->GetTextureCount(textureType);

	if (textureCount)
	{
		texturePaths.reserve(textureCount);

		for (std::uint32_t index = 0u; index < textureCount; ++index)
		{
			aiString aiTexturePath{};

			material->GetTexture(textureType, index, &aiTexturePath);

			TexturePath path{ .path = fileDirectory + aiTexturePath.C_Str() };

			path.name = SceneProcessor::GetFileName(path.path);

			texturePaths.emplace_back(std::move(path));
		}
	}
}

void SceneMaterialProcessor::LoadBlinnPhongMaterials(
	BlinnPhongLightTechnique& blinnPhongTechnique
) {
	const size_t materialCount = std::size(m_materialData);

	for (size_t index = 0u; index < materialCount; ++index)
		m_materialDetails[index].materialIndex = blinnPhongTechnique.AddMaterial(
			m_materialData[index]
		);
}

std::pair<std::uint32_t, std::uint32_t> SceneMaterialProcessor::CreateAndAddAtlas(
	TextureAtlas& atlas, Renderer& renderer
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

void SceneMaterialProcessor::ConfigureMaterialTextures(
	std::vector<MeshTextureDetails>& textureDetails, const std::vector<TexturePath>& texturePaths,
	TextureAtlas& atlas, std::uint32_t atlasIndex, std::uint32_t atlasBindIndex
) noexcept {
	const size_t textureCount = std::size(texturePaths);

	textureDetails.reserve(textureCount);

	for (size_t index = 0u; index < textureCount; ++index)
	{
		const TexturePath& texturePath = texturePaths[index];

		textureDetails.emplace_back(
			MeshTextureDetails
			{
				.textureIndex     = atlasIndex,
				.textureBindIndex = atlasBindIndex,
				.uvInfo           = atlas.GetUVInfo(texturePath.name)
			}
		);
	}
}

void SceneMaterialProcessor::ConfigureMaterialTextures(
	std::vector<MeshTextureDetails>& textureDetails,
	const std::vector<TexturePath>& texturePaths, Renderer& renderer
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

void SceneMaterialProcessor::LoadTexturesAsAtlas(Renderer& renderer)
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

	auto [diffuseTextureIndex, diffuseBindingIndex]   = CreateAndAddAtlas(diffuseAtlas, renderer);
	auto [specularTextureIndex, specularBindingIndex] = CreateAndAddAtlas(specularAtlas, renderer);

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

void SceneMaterialProcessor::LoadTextures(Renderer& renderer)
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
