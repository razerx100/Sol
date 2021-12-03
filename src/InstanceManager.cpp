#include <InstanceManager.hpp>
#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>

void AppInst::Init() {
	Set(CreateAppInstance());
}

void IOInst::Init(IOType type) {
	if (type == IOType::Pluto)
		Set(CreatePlutoInstance());
}

void WindowInst::Init(
	int width, int height, InputManager* ioMan, const char* name,
	WindowType type
) {
	if (type == WindowType::Luna)
		Set(CreateLunaInstance(
			width, height, ioMan, name
		));
}

void RendererInst::Init(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	RendererType type,
	std::uint8_t bufferCount
) {
	if (type == RendererType::Gaia)
		Set(
			CreateGaiaInstance(
				appName,
				windowHandle,
				width, height,
				bufferCount
			)
		);
	else if (type == RendererType::Terra)
		Set(
			CreateTerraInstance(
				appName,
				windowHandle, moduleHandle,
				width, height,
				bufferCount
			)
		);
}
