#ifndef CONFIG_MANAGER_HPP_
#define CONFIG_MANAGER_HPP_
#include <IniParser.hpp>
#include <array>
#include <optional>
#include <ModuleTypes.hpp>

namespace Sol
{
class ConfigManager
{
public:
	ConfigManager(std::wstring fileName) : m_parser{ std::move(fileName) }
	{
		ReadConfigFile();
	}
	~ConfigManager() noexcept { m_parser.WriteBack(); }

	void ReadConfigFile() noexcept;

	void SetRenderer(const std::string& name) noexcept
	{
		m_parser.AddOrUpdateValue("Modules", "Renderer", name);
	}
	void SetIO(const std::string& name) noexcept
	{
		m_parser.AddOrUpdateValue("Modules", "IO", name);
	}
	void SetWindow(const std::string& name) noexcept
	{
		m_parser.AddOrUpdateValue("Modules", "Window", name);
	}
	void SetRenderEngine(const std::string& name) noexcept
	{
		m_parser.AddOrUpdateValue("Systems", "RenderEngine", name);
	}

	[[nodiscard]]
	std::string GetRendererName() const noexcept
	{
		return m_parser.GetValue("Renderer", "Modules");
	}
	[[nodiscard]]
	RendererModule GetRendererModuleType() const noexcept;
	[[nodiscard]]
	RenderEngineType GetRenderEngineType() const noexcept;
	[[nodiscard]]
	WindowModule GetWindowModuleType() const noexcept;

	[[nodiscard]]
	std::string GeIOName() const noexcept
	{
		return m_parser.GetValue("IO", "Modules");
	}
	[[nodiscard]]
	std::string GetWindowName() const noexcept
	{
		return m_parser.GetValue("Window", "Modules");
	}
	[[nodiscard]]
	std::string GetRenderEngine() const noexcept
	{
		return m_parser.GetValue("RenderEngine", "Systems");
	}

private:
	IniParser m_parser;

public:
	ConfigManager(const ConfigManager& other) noexcept
		: m_parser{ other.m_parser }
	{}
	ConfigManager& operator=(const ConfigManager& other) noexcept
	{
		m_parser = other.m_parser;

		return *this;
	}

	ConfigManager(ConfigManager&& other) noexcept
		: m_parser{ std::move(other.m_parser) }
	{}
	ConfigManager& operator=(ConfigManager&& other) noexcept
	{
		m_parser = std::move(other.m_parser);

		return *this;
	}
};
}
#endif
