#include <Sol1.hpp>

#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>


Sol::Sol(const Args& arguments)
	: m_appName{ arguments.appName.value() }, m_objManager{},
	m_configManager{ nullptr }, m_threadPool{ nullptr },
	m_sharedData{ nullptr }, m_app { nullptr }
{
	const std::uint32_t width = arguments.width.value();
	const std::uint32_t height = arguments.height.value();

	m_objManager.CreateObject(m_configManager, { L"config.ini" }, 0u);
	Config().ReadConfigFile();

	/*m_objManager.CreateObject(
		m_threadPool, std::make_unique<ThreadPool>(arguments.threadCount.value()), 3u
	);*/
	m_objManager.CreateObject<SharedDataContainer>(m_sharedData, 3u);

	InitIO(Config().GeIOName());
	IO().AddGamepadSupport(arguments.gamepadCount.value());

	m_objManager.CreateObject(m_app, 1u);
}

void Sol::InitIO(std::string_view moduleName)
{
	if (moduleName == "Pluto")
		m_objManager.CreateObject(m_io, CreatePlutoInstance(), 3u);
}
