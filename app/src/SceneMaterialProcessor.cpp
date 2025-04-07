#include <SceneMaterialProcessor.hpp>
#include <ConversionUtilities.hpp>
#include <TextureAtlas.hpp>
#include <array>
#include <ranges>
#include <algorithm>
#include <assimp/GltfMaterial.h>

namespace PSOIndexMap
{
	// Shader Type indices
	static std::array<std::uint32_t, static_cast<size_t>(ShaderType::Count)> s_pipelineIndexMap{};

	void SetPipelineIndex(ShaderType type, std::uint32_t psoIndex) noexcept
	{
		s_pipelineIndexMap[static_cast<size_t>(type)] = psoIndex;
	}

	std::uint32_t GetPipelineIndex(ShaderType type) noexcept
	{
		return s_pipelineIndexMap[static_cast<size_t>(type)];
	}
}

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
		aiColor3D diffuse{ 1.f, 1.f, 1.f };
		aiColor3D specular{ 0.f, 0.f, 0.f };
		aiColor3D ambient{ 0.f, 0.f, 0.f };
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

		m_materialDetails.emplace_back(
			MaterialDetails
			{
				.name          = name.C_Str(),
				.pipelineIndex = PSOIndexMap::GetPipelineIndex(
					isTransparent ? ShaderType::TransparentLight : ShaderType::OpaqueLight
				)
			}
		);

		// Diffuse
		constexpr aiTextureType diffuseType = aiTextureType_DIFFUSE;

		const auto diffuseCount = material->GetTextureCount(diffuseType);

		if (diffuseCount)
		{
			// Texture
			aiString aiTexturePath{};

			// Only process the first texture for now.
			material->GetTexture(diffuseType, 0u, &aiTexturePath);

			m_texturePaths.emplace_back(
				TexturePath
				{
					.diffuse = fileDirectory + aiTexturePath.C_Str()
				}
			);
			m_baseTextureDetails.emplace_back(
				TextureDetails{ .materialIndex = static_cast<std::uint32_t>(index) }
			);
		}
	}
}

void SceneMaterialProcessor::LoadBlinnPhongMaterials(BlinnPhongLightTechnique& blinnPhongTechnique)
{
	const size_t materialCount = std::size(m_materialData);

	for (size_t index = 0u; index < materialCount; ++index)
		m_materialDetails[index].materialIndex = blinnPhongTechnique.AddMaterial(
			m_materialData[index]
		);
}

void SceneMaterialProcessor::LoadTexturesAsAtlas(Renderer& renderer)
{
	const size_t textureCount = std::size(m_texturePaths);

	TextureAtlas baseColourAtlas{};

	// Maybe later I will make it so there are multiple atlas and only a certain number of
	// textures are put into a single atlas. As the precision would decrease the more textures
	// we put in one.
	for (size_t index = 0u; index < textureCount; ++index)
	{
		const TexturePath& texturePath     = m_texturePaths[index];

		// Base colour
		const std::string& baseTexturePath = texturePath.diffuse;
		const std::string baseName         = SceneProcessor::GetFileName(baseTexturePath);

		m_baseTextureDetails[index].name   = baseName;

		baseColourAtlas.AddTexture(baseName, baseTexturePath);
	}

	if (baseColourAtlas.DoUnprocessedTexturesExist())
	{
		baseColourAtlas.CreateAtlas();

		const auto baseTextureIndex     = static_cast<std::uint32_t>(
			renderer.AddTexture(std::move(baseColourAtlas.MoveTexture()))
		);
		const auto baseTextureBindIndex = static_cast<std::uint32_t>(
			renderer.BindTexture(baseTextureIndex)
		);

		for (size_t index = 0u; index < textureCount; ++index)
		{
			TextureDetails& baseTextureDetails  = m_baseTextureDetails[index];

			baseTextureDetails.textureIndex     = baseTextureIndex;
			baseTextureDetails.textureBindIndex = baseTextureBindIndex;
			baseTextureDetails.uvInfo           = baseColourAtlas.GetUVInfo(baseTextureDetails.name);
		}
	}
}

void SceneMaterialProcessor::LoadTextures(Renderer& renderer)
{
	const size_t textureCount = std::size(m_texturePaths);

	for (size_t index = 0u; index < textureCount; ++index)
	{
		const TexturePath& texturePath      = m_texturePaths[index];

		// Base colour
		const std::string& baseTexturePath  = texturePath.diffuse;

		std::optional<STexture> baseTexture = TextureTool::LoadTextureFromFile(baseTexturePath);

		if (baseTexture)
		{
			const auto baseTextureIndex = static_cast<std::uint32_t>(
				renderer.AddTexture(std::move(*baseTexture))
			);
			const auto baseTextureBindIndex = static_cast<std::uint32_t>(
				renderer.BindTexture(baseTextureIndex)
			);

			TextureDetails& baseTextureDetails  = m_baseTextureDetails[index];

			baseTextureDetails.name             = SceneProcessor::GetFileName(baseTexturePath);
			baseTextureDetails.textureIndex     = baseTextureIndex;
			baseTextureDetails.textureBindIndex = baseTextureBindIndex;

			// The UV info will be the default one for non atlas textures, so no need to set it.
		}
	}
}

const SceneMaterialProcessor::TextureDetails& SceneMaterialProcessor::GetBaseTextureDetails(
	const std::string& fileName
) const noexcept {
	auto result = std::ranges::find(
		m_baseTextureDetails, fileName, [](const TextureDetails& details) { return details.name; }
	);

	if (std::end(m_baseTextureDetails) != result)
		return *result;

	return m_defaultTextureDetails;
}

const SceneMaterialProcessor::TextureDetails& SceneMaterialProcessor::GetBaseTextureDetails(
	size_t materialIndex
) const noexcept {
	auto result = std::ranges::find(
		m_baseTextureDetails, materialIndex, [](const TextureDetails& details)
		{ return details.materialIndex; }
	);

	if (std::end(m_baseTextureDetails) != result)
		return *result;

	return m_defaultTextureDetails;
}
