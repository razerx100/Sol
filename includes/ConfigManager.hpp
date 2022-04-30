#ifndef CONFIG_MANAGER_HPP_
#define CONFIG_MANAGER_HPP_
#include <IniParser.hpp>
#include <array>

class ConfigManager {
	const std::unordered_map<std::string, std::string> DEFAULTMODULES = {
		{"Renderer", "Gaia"},
		{"Window", "Luna"},
		{"IO", "Pluto"}
	};

public:
	ConfigManager(const std::string& fileName);
	~ConfigManager() noexcept;

	void ReadConfigFile() noexcept;

	void SetRenderer(const std::string& name) noexcept;
	void SetIO(const std::string& name) noexcept;
	void SetWindow(const std::string& name) noexcept;

	[[nodiscard]]
	std::string GetRendererName() const noexcept;
	[[nodiscard]]
	std::string GeIOName() const noexcept;
	[[nodiscard]]
	std::string GetWindowName() const noexcept;

private:
	IniParser m_parser;
};
#endif
