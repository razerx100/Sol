#include <SceneProcessor.hpp>
#include <SolException.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

SceneProcessor::SceneProcessor(const std::string& scenePath)
	: m_scene{
		aiImportFile(
			scenePath.c_str(),
			static_cast<std::uint32_t>(
			aiProcess_CalcTangentSpace         |
			aiProcess_Triangulate              |
			aiProcess_JoinIdenticalVertices    |
			aiProcess_MakeLeftHanded           |
			aiProcess_FlipUVs                  |
			aiProcess_FlipWindingOrder         |
			aiProcess_GenBoundingBoxes         |
			aiProcess_GenNormals               |
			aiProcess_GenUVCoords              |
			aiProcess_SortByPType              |
			aiProcess_RemoveRedundantMaterials
			)
		)
	}, m_filePath{ scenePath }
{
	if (!m_scene)
		throw Exception{ "SceneFileMissing", "The scene file couldn't be found." };
}

SceneProcessor::~SceneProcessor() noexcept
{
	if (m_scene)
		aiReleaseImport(m_scene);
}

size_t SceneProcessor::GetFileNameCount(const std::string& filePath) noexcept
{
	size_t fileNameCount = 0u;

	// Having the index as an unsigned integer is UB, as it would never go below 0.
	const auto characterCount = static_cast<std::int32_t>(std::size(filePath));

	for (std::int32_t index = characterCount - 1; index >= 0; --index)
	{
		if (filePath[index] == '/')
			break;

		++fileNameCount;
	}

	return fileNameCount;
}

std::string SceneProcessor::GetFileName(const std::string& filePath) noexcept
{
	const size_t characterCount = std::size(filePath);

	std::string fileName{};

	if (!std::empty(filePath))
	{
		size_t fileNameCount = GetFileNameCount(filePath);

		fileName             = filePath.substr(characterCount - fileNameCount, fileNameCount);
	}

	return fileName;
}

std::string SceneProcessor::GetFileDirectory(const std::string& filePath) noexcept
{
	const size_t characterCount = std::size(filePath);

	std::string directoryPath{};

	if (!std::empty(filePath))
	{
		size_t fileNameCount = GetFileNameCount(filePath);

		directoryPath        = filePath.substr(0u, characterCount - fileNameCount);
	}

	return directoryPath;
}
