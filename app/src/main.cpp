#include <memory>
#include <Sol.hpp>
#include <ConfigManager.hpp>
#include <SolException.hpp>
#include <ExceptionMessageBox.hpp>

using namespace Sol;

template<RendererModule module_t, class Engine_t>
[[nodiscard]]
static int RunSol(ConfigManager&& configManager)
{
	using Sol_t = ::Sol::Sol<module_t, Engine_t>;

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

template<RendererModule module_t>
[[nodiscard]]
static int ConfigureRenderEngineAndRunSol(
	ConfigManager&& configManager, RenderEngineType engineType
) {
	using enum RenderEngineType;

	if (engineType == MeshDraw)
		return RunSol<module_t, typename RenderEngineConditional<module_t, MeshDraw>::type>(
			std::move(configManager)
		);
	else if (engineType == IndirectDraw)
		return RunSol<module_t, typename RenderEngineConditional<module_t, IndirectDraw>::type>(
			std::move(configManager)
		);
	else
		return RunSol<module_t, typename RenderEngineConditional<module_t, IndividualDraw>::type>(
			std::move(configManager)
		);
}

[[nodiscard]]
static int ConfigureRendererAndRunSol(
	ConfigManager&& configManager, RendererModule moduleType, RenderEngineType engineType
) {
	if (moduleType == RendererModule::Gaia)
		return ConfigureRenderEngineAndRunSol<RendererModule::Gaia>(
			std::move(configManager), engineType
		);
	else
		return ConfigureRenderEngineAndRunSol<RendererModule::Terra>(
			std::move(configManager), engineType
		);
}

int main()
{
	try
	{
		ConfigManager configManager{ L"config.ini" };

		const RendererModule moduleType   = configManager.GetRendererModuleType();

		const RenderEngineType engineType = configManager.GetRenderEngineType();

		return ConfigureRendererAndRunSol(std::move(configManager), moduleType, engineType);
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
