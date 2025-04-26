#ifndef SOL_CONFIGURATOR_HPP_
#define SOL_CONFIGURATOR_HPP_
#include <memory>
#include <Sol.hpp>
#include <ConfigManager.hpp>

namespace Sol
{
template<
	RendererModule rendererModule_t,
	WindowModule windowModule_t,
	InputModule inputModule_t,
	class Engine_t,
	class App_t
>
[[nodiscard]]
int RunSol(ConfigManager&& configManager)
{
	using Sol_t = ::Sol::Sol<rendererModule_t, windowModule_t, inputModule_t, Engine_t, App_t>;

	auto sol = std::make_unique<Sol_t>("Sol", std::move(configManager));

	return sol->Run();
}

template<RendererModule renderModule_t, RenderEngineType renderEngine_t>
struct RenderEngineConditional { using type = Terra::RenderEngineVSIndividual; };

template<>
struct RenderEngineConditional<RendererModule::Terra, RenderEngineType::IndirectDraw>
{
	using type = Terra::RenderEngineVSIndirect;
};

template<>
struct RenderEngineConditional<RendererModule::Terra, RenderEngineType::MeshDraw>
{
	using type = Terra::RenderEngineMS;
};

template<>
struct RenderEngineConditional<RendererModule::Gaia, RenderEngineType::IndividualDraw>
{
	using type = Gaia::RenderEngineVSIndividual;
};

template<>
struct RenderEngineConditional<RendererModule::Gaia, RenderEngineType::IndirectDraw>
{
	using type = Gaia::RenderEngineVSIndirect;
};

template<>
struct RenderEngineConditional<RendererModule::Gaia, RenderEngineType::MeshDraw>
{
	using type = Gaia::RenderEngineMS;
};

template<
	RendererModule rendererModule_t,
	WindowModule windowModule_t,
	InputModule inputModule_t,
	class App_t
>
[[nodiscard]]
int ConfigureRenderEngineAndRunSol(
	ConfigManager&& configManager, RenderEngineType engineType
) {
	using enum RenderEngineType;

	if (engineType == MeshDraw)
		return RunSol<
			rendererModule_t,
			windowModule_t,
			inputModule_t,
			typename RenderEngineConditional<rendererModule_t, MeshDraw>::type,
			App_t
		>(std::move(configManager));
	else if (engineType == IndirectDraw)
		return RunSol<
			rendererModule_t,
			windowModule_t,
			inputModule_t,
			typename RenderEngineConditional<rendererModule_t, IndirectDraw>::type,
			App_t
		>(std::move(configManager));
	else
		return RunSol<
			rendererModule_t,
			windowModule_t,
			inputModule_t,
			typename RenderEngineConditional<rendererModule_t, IndividualDraw>::type,
			App_t
		>(std::move(configManager));
}

template<WindowModule windowModule_t, InputModule inputModule_t, class App_t>
[[nodiscard]]
int ConfigureRendererAndRunSol(
	ConfigManager&& configManager, RendererModule rendererModuleType, RenderEngineType engineType
) {
	if (rendererModuleType == RendererModule::Gaia)
		return ConfigureRenderEngineAndRunSol<
			RendererModule::Gaia,
			windowModule_t,
			inputModule_t,
			App_t
		>(std::move(configManager), engineType);
	else
		return ConfigureRenderEngineAndRunSol<
			RendererModule::Terra,
			windowModule_t,
			inputModule_t,
			App_t
		>(std::move(configManager), engineType);
}

template<InputModule inputModule_t, class App_t>
[[nodiscard]]
int ConfigureWindowRendererAndRunSol(
	ConfigManager&& configManager, RendererModule rendererModuleType,
	RenderEngineType engineType, [[maybe_unused]] WindowModule windowModuleType
) {
	return ConfigureRendererAndRunSol<WindowModule::Luna, inputModule_t, App_t>(
		std::move(configManager), rendererModuleType, engineType
	);
}

template<class App_t>
[[nodiscard]]
int ConfigureInputWindowRendererAndRunSol(
	ConfigManager&& configManager, RendererModule rendererModuleType,
	RenderEngineType engineType, WindowModule windowModuleType,
	[[maybe_unused]] InputModule inputModuleType
) {
	return ConfigureWindowRendererAndRunSol<InputModule::Pluto, App_t>(
		std::move(configManager), rendererModuleType, engineType, windowModuleType
	);
}

template<class App_t>
[[nodiscard]]
int ConfigureAppAndRunSol(ConfigManager&& configManager)
{
	const RendererModule rendererModuleType = configManager.GetRendererModuleType();

	const RenderEngineType engineType       = configManager.GetRenderEngineType();

	const WindowModule windowModuleType     = configManager.GetWindowModuleType();

	const InputModule inputModuleType       = configManager.GetInputModuleType();

	return ConfigureInputWindowRendererAndRunSol<App_t>(
		std::move(configManager), rendererModuleType, engineType, windowModuleType,
		inputModuleType
	);
}
}
#endif
