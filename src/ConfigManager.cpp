#include <ConfigManager.hpp>

ConfigManager::ConfigManager(Args& arguments)
	: m_parser{ std::move(arguments.fileName.value()) } {}

ConfigManager::~ConfigManager() noexcept {
	m_parser.WriteBack();
}

void ConfigManager::ReadConfigFile() noexcept {
	m_parser.Parse();

	if (!m_parser.DoesValueExist("Renderer", "Modules")) {
		auto valueMap = DEFAULTMODULES.find("Renderer");

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue("Modules", valueMap->first, valueMap->second);
	}

	if (!m_parser.DoesValueExist("Window", "Modules")) {
		auto valueMap = DEFAULTMODULES.find("Window");

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue("Modules", valueMap->first, valueMap->second);
	}

	if (!m_parser.DoesValueExist("IO", "Modules")) {
		auto valueMap = DEFAULTMODULES.find("IO");

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue("Modules", valueMap->first, valueMap->second);
	}
}

void ConfigManager::SetRenderer(const std::string& name) noexcept {
	m_parser.AddOrUpdateValue("Modules", "Renderer", name);
}

void ConfigManager::SetIO(const std::string& name) noexcept {
	m_parser.AddOrUpdateValue("Modules", "IO", name);
}

void ConfigManager::SetWindow(const std::string& name) noexcept {
	m_parser.AddOrUpdateValue("Modules", "Window", name);
}

std::string ConfigManager::GetRendererName() const noexcept {
	return m_parser.GetValue("Renderer", "Modules");
}

std::string ConfigManager::GeIOName() const noexcept {
	return m_parser.GetValue("IO", "Modules");
}

std::string ConfigManager::GetWindowName() const noexcept {
	return m_parser.GetValue("Window", "Modules");
}
