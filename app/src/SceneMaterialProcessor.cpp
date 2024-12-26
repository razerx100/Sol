#include <SceneMaterialProcessor.hpp>
#include <ConversionUtilities.hpp>
#include <MaterialBase.hpp>
#include <TextureAtlas.hpp>

void SceneMaterialProcessor::ProcessMeshAndMaterialData()
{
	aiScene const* scene = m_scene->GetScene();

	const size_t materialCount = scene->mNumMaterials;
	aiMaterial** materials     = scene->mMaterials;

	const std::string fileDirectory = m_scene->GetFileDirectory();

	for (size_t index = 0u; index < materialCount; ++index)
	{
		aiMaterial* material = materials[index];

		constexpr aiTextureType baseColour = aiTextureType_BASE_COLOR;

		if (material->GetTextureCount(baseColour))
		{
			// Texture
			aiString aiTexturePath{};

			// Only process the first texture for now.
			material->GetTexture(baseColour, 0u, &aiTexturePath);

			m_texturePaths.emplace_back(
				TexturePath
				{
					.baseColour = fileDirectory + aiTexturePath.C_Str()
				}
			);

			// Material
			aiColor3D diffuse{ 1.f, 1.f, 1.f };
			aiColor3D specular{ 0.f, 0.f, 0.f };
			aiColor3D ambient{ 0.f, 0.f, 0.f };
			float shininess = 1.f;

			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);

			m_materialData.emplace_back(
				MaterialData
				{
					.ambient   = GetXMFloat4(ambient),
					.diffuse   = GetXMFloat4(diffuse),
					.specular  = GetXMFloat4(specular),
					.shininess = shininess
				}
			);

			m_textureDetails.emplace_back();
		}
	}
}

void SceneMaterialProcessor::LoadMaterials(Renderer& renderer)
{
	const size_t materialCount = std::size(m_materialData);

	for (size_t index = 0u; index < materialCount; ++index)
	{
		const size_t materialIndex = renderer.AddMaterial(
			std::make_shared<MaterialBase>(m_materialData[index])
		);

		m_textureDetails[index].materialIndex = static_cast<std::uint32_t>(materialIndex);
	}
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
		const std::string& baseTexturePath = texturePath.baseColour;
		const std::string baseName         = SceneProcessor::GetFileName(baseTexturePath);

		m_textureDetails[index].baseName   = baseName;

		baseColourAtlas.AddTexture(baseName, baseTexturePath);
	}

	if (baseColourAtlas.DoUnprocessedTexturesExist())
	{
		baseColourAtlas.CreateAtlas();

		const auto baseTextureIndex = static_cast<std::uint32_t>(
			renderer.AddTexture(std::move(baseColourAtlas.MoveTexture()))
		);

		for (size_t index = 0u; index < textureCount; ++index)
		{
			TextureDetails& textureDetails  = m_textureDetails[index];
			textureDetails.baseTextureIndex = baseTextureIndex;
			textureDetails.uvInfo           = baseColourAtlas.GetUVInfo(textureDetails.baseName);
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
		const std::string& baseTexturePath  = texturePath.baseColour;

		std::optional<STexture> baseTexture = TextureTool::LoadTextureFromFile(baseTexturePath);

		if (baseTexture)
		{
			const size_t baseTextureIndex   = renderer.AddTexture(std::move(*baseTexture));

			TextureDetails& textureDetails  = m_textureDetails[index];
			textureDetails.baseName         = SceneProcessor::GetFileName(baseTexturePath);
			textureDetails.baseTextureIndex = static_cast<std::uint32_t>(baseTextureIndex);

			// The UV info will be the default one for non atlas textures, so no need to set it.
		}
	}
}
