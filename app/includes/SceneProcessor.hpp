#ifndef SCENE_PROCESSOR_HPP_
#define SCENE_PROCESSOR_HPP_
#include <assimp/scene.h>
#include <string>

class SceneProcessor
{
public:
	SceneProcessor(const std::string& scenePath);
	~SceneProcessor() noexcept;

	[[nodiscard]]
	std::string GetFileDirectory() const noexcept { return GetFileDirectory(m_filePath); }
	[[nodiscard]]
	std::string GetFileName() const noexcept { return GetFileName(m_filePath); }

	[[nodiscard]]
	aiScene const* GetScene() const noexcept { return m_scene; }

	[[nodiscard]]
	static std::string GetFileName(const std::string& filePath) noexcept;

private:
	[[nodiscard]]
	static std::string GetFileDirectory(const std::string& filePath) noexcept;
	[[nodiscard]]
	static size_t GetFileNameCount(const std::string& filePath) noexcept;

private:
	aiScene const* m_scene;
	std::string    m_filePath;

public:
	SceneProcessor(const SceneProcessor&) = delete;
	SceneProcessor& operator=(const SceneProcessor&) = delete;

	SceneProcessor(SceneProcessor&& other) noexcept
		: m_scene{ std::exchange(other.m_scene, nullptr) },
		m_filePath{ std::move(other.m_filePath) }
	{}
	SceneProcessor& operator=(SceneProcessor&& other) noexcept
	{
		m_scene    = std::exchange(other.m_scene, nullptr);
		m_filePath = std::move(other.m_filePath);

		return *this;
	}
};
#endif
