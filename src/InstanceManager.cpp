#include <InstanceManager.hpp>
#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <vector>
#include <ConfigManager.hpp>

void AppInst::Init() {
	Set(CreateAppInstance());
}

void IOInst::Init(IOType type) {
	if (type == IOType::Pluto)
		Set(CreatePlutoInstance());
}

void WindowInst::Init(
	std::uint32_t width, std::uint32_t height, InputManager* ioMan, const char* name,
	WindowType type
) {
	if (type == WindowType::Luna)
		Set(CreateLunaInstance(
			width, height, ioMan, name
		));
}

static const std::vector<const char*> RENDERERNAMES = {
	"Terra",
	"Gaia"
};

void RendererInst::Init(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	RendererType type,
	std::uint8_t bufferCount
) {
	type = ConfigManager::ReadType("config.ini", "RendererType", type, RENDERERNAMES);

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

void RendererInst::SetAPI(RendererType type) noexcept {
	ConfigManager::SaveName("config.ini", "RendererType", type, RENDERERNAMES);
}

void ModelContInst::Init() {
	Set(
		CreateModelContainerInstance()
	);
}

void UploadBufferInst::Init() {
	Set(
		CreateUploadBufferInstance()
	);
}
