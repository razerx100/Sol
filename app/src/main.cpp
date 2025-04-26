#include <memory>
#include <Sol.hpp>
#include <ConfigManager.hpp>
#include <SolException.hpp>
#include <ExceptionMessageBox.hpp>

template<RendererModule module_t, class Engine_t>
int RunSol(ConfigManager&& configManager)
{
	using Sol_t = Sol<module_t, Engine_t>;

	auto sol = std::make_unique<Sol_t>("Sol", std::move(configManager));

	return sol->Run();
}

int main()
{
	try
	{
		ConfigManager configManager{ L"config.ini" };

		const RenderEngineType engineType = configManager.GetRenderEngineType();

		if (configManager.GetRendererName() == "Gaia")
		{
			constexpr RendererModule renderModule_t = RendererModule::Gaia;

			if (engineType == RenderEngineType::MeshDraw)
				return RunSol<renderModule_t, Gaia::RenderEngineMS>(std::move(configManager));
			else if (engineType == RenderEngineType::IndirectDraw)
				return RunSol<renderModule_t, Gaia::RenderEngineVSIndirect>(
					std::move(configManager)
				);
			else
				return RunSol<renderModule_t, Gaia::RenderEngineVSIndividual>(
					std::move(configManager)
				);
		}
		else
		{
			constexpr RendererModule renderModule_t = RendererModule::Terra;

			if (engineType == RenderEngineType::MeshDraw)
				return RunSol<renderModule_t, Terra::RenderEngineMS>(std::move(configManager));
			else if (engineType == RenderEngineType::IndirectDraw)
				return RunSol<renderModule_t, Terra::RenderEngineVSIndirect>(
					std::move(configManager)
				);
			else
				return RunSol<renderModule_t, Terra::RenderEngineVSIndividual>(
					std::move(configManager)
				);
		}
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
