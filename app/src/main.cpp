#include <memory>
#include <Sol.hpp>
#include <ConfigManager.hpp>
#include <SolException.hpp>
#include <ExceptionMessageBox.hpp>

using namespace Sol;

template<RendererModule rendererModule_t, WindowModule windowModule_t, class Engine_t>
[[nodiscard]]
static int RunSol(ConfigManager&& configManager)
{
	using Sol_t = ::Sol::Sol<rendererModule_t, windowModule_t, Engine_t>;

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

template<RendererModule rendererModule_t, WindowModule windowModule_t>
[[nodiscard]]
static int ConfigureRenderEngineAndRunSol(
	ConfigManager&& configManager, RenderEngineType engineType
) {
	using enum RenderEngineType;

	if (engineType == MeshDraw)
		return RunSol<
			rendererModule_t,
			windowModule_t,
			typename RenderEngineConditional<rendererModule_t, MeshDraw>::type
		>(std::move(configManager));
	else if (engineType == IndirectDraw)
		return RunSol<
			rendererModule_t,
			windowModule_t,
			typename RenderEngineConditional<rendererModule_t, IndirectDraw>::type
		>(std::move(configManager));
	else
		return RunSol<
			rendererModule_t,
			windowModule_t,
			typename RenderEngineConditional<rendererModule_t, IndividualDraw>::type
		>(std::move(configManager));
}

template<WindowModule windowModule_t>
[[nodiscard]]
static int ConfigureRendererAndRunSol(
	ConfigManager&& configManager, RendererModule rendererModuleType, RenderEngineType engineType
) {
	if (rendererModuleType == RendererModule::Gaia)
		return ConfigureRenderEngineAndRunSol<
			RendererModule::Gaia, windowModule_t
		>(std::move(configManager), engineType);
	else
		return ConfigureRenderEngineAndRunSol<
			RendererModule::Terra, windowModule_t
		>(std::move(configManager), engineType);
}

[[nodiscard]]
static int ConfigureWindowRendererAndRunSol(
	ConfigManager&& configManager, RendererModule rendererModuleType,
	RenderEngineType engineType, [[maybe_unused]] WindowModule windowModuleType
) {
	return ConfigureRendererAndRunSol<WindowModule::Luna>(
		std::move(configManager), rendererModuleType, engineType
	);
}

int main()
{
	try
	{
		ConfigManager configManager{ L"config.ini" };

		const RendererModule rendererModuleType = configManager.GetRendererModuleType();

		const RenderEngineType engineType       = configManager.GetRenderEngineType();

		const WindowModule windowModuleType     = configManager.GetWindowModuleType();

		return ConfigureWindowRendererAndRunSol(
			std::move(configManager), rendererModuleType, engineType, windowModuleType
		);
	}
	catch (const Exception& e)
	{
		ExceptionMessageBox(e.what(), e.GetType());
	}
	catch (const std::exception& e)
	{
		ExceptionMessageBox(e.what(), "Standard Exception");
	}
	catch (...)
	{
		ExceptionMessageBox("No details available", "Unknown Exception");
	}

	return -1;
}
