#ifndef CONFIG_MANAGER_HPP_
#define CONFIG_MANAGER_HPP_
#include <IniParser.hpp>
#include <array>
#include <optional>
#include <RendererTypes.hpp>

class ConfigManager
{
	const std::unordered_map<std::string, std::string> DEFAULTMODULES =
	{
		{"Renderer",     "Terra"},
		{"RenderEngine", "IndividualDraw"},
		{"Window",       "Luna"},
		{"IO",           "Pluto"}
	};

public:
	struct Args
	{
		std::optional<std::wstring> fileName;
	};

public:
	ConfigManager(Args& arguments);
	~ConfigManager() noexcept { m_parser.WriteBack(); }

	void ReadConfigFile() noexcept;

	void SetRenderer(const std::string& name) noexcept;
	void SetIO(const std::string& name) noexcept;
	void SetWindow(const std::string& name) noexcept;
	void SetRenderEngine(const std::string& name) noexcept;

	[[nodiscard]]
	std::string GetRendererName() const noexcept;
	[[nodiscard]]
	RenderEngineType GetRenderEngineType() const noexcept;
	[[nodiscard]]
	std::string GeIOName() const noexcept;
	[[nodiscard]]
	std::string GetWindowName() const noexcept;
	[[nodiscard]]
	std::string GetRenderEngine() const noexcept;

private:
	IniParser m_parser;
};
#endif
