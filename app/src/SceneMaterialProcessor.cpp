#include <SceneMaterialProcessor.hpp>
#include <ConversionUtilities.hpp>
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
		aiColor4D diffuse{ 0.8f, 0.8f, 0.8f, 0.8f };
		aiColor4D specular{ 0.3f, 0.3f, 0.3f, 0.3f };
		aiColor4D ambient{ 0.2f, 0.2f, 0.2f, 0.2f };
		float shininess = 16.f;
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
