#include <SolConfigurator.hpp>
#include <SolException.hpp>
#include <ExceptionMessageBox.hpp>
#include <App.hpp>

int main()
{
	try
	{
		Sol::ConfigManager configManager{ L"config.ini" };

		using App_t = ExampleApp::App<Sol::InputModule::Pluto>;

		return Sol::ConfigureAppAndRunSol<App_t>(std::move(configManager));
	}
	catch (const Sol::Exception& e)
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
