#include <InstanceManager.hpp>
#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>

void AppInst::Init() noexcept {
	Set(CreateAppInstance());
}

void IOInst::Init(IOType type) noexcept {
	if (type == IOType::Pluto)
		Set(CreatePlutoInstance());
}

void WindowInst::Init(
	int width, int height, InputManager* ioMan, const char* name,
	WindowType type
) noexcept {
	if (type == WindowType::Luna)
		Set(CreateLunaInstance(
			width, height, ioMan, name
		));
}
