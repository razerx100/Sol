#include <ConfigManager.hpp>

ConfigManager::ConfigManager(Args& arguments)
	: m_parser{ std::move(arguments.fileName.value()) }
{}

void ConfigManager::ReadConfigFile() noexcept
{
	m_parser.Parse();

	const std::string modules = "Modules";

	if (std::string renderer = "Renderer"; !m_parser.DoesValueExist(renderer, modules)) {
		auto valueMap = DEFAULTMODULES.find(renderer);

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue(modules, valueMap->first, valueMap->second);
	}

	if (std::string window = "Window"; !m_parser.DoesValueExist(window, modules)) {
		auto valueMap = DEFAULTMODULES.find(window);

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue(modules, valueMap->first, valueMap->second);
	}

	if (std::string io = "IO"; !m_parser.DoesValueExist(io, modules)) {
		auto valueMap = DEFAULTMODULES.find(io);

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue(modules, valueMap->first, valueMap->second);
	}

	const std::string systems = "Systems";
	if (std::string renderEngine = "RenderEngine";
		!m_parser.DoesValueExist(renderEngine, systems)) {
		auto valueMap = DEFAULTMODULES.find(renderEngine);

		if (valueMap != std::end(DEFAULTMODULES))
			m_parser.AddOrUpdateValue(systems, valueMap->first, valueMap->second);
	}
}

RenderEngineType ConfigManager::GetRenderEngineType() const noexcept
{
	RenderEngineType renderEngineType = RenderEngineType::IndividualDraw;

	const std::string engineType = GetRenderEngine();

	if (engineType == "IndirectDraw")
		renderEngineType = RenderEngineType::IndirectDraw;
	else if (engineType == "IndividualDraw")
		renderEngineType = RenderEngineType::IndividualDraw;
	else if (engineType == "MeshDraw")
		renderEngineType = RenderEngineType::MeshDraw;

	return renderEngineType;
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

void ConfigManager::SetRenderEngine(const std::string& name) noexcept {
	m_parser.AddOrUpdateValue("Systems", "RenderEngine", name);
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

std::string ConfigManager::GetRenderEngine() const noexcept {
	return m_parser.GetValue("RenderEngine", "Systems");
}
