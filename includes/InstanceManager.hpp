#ifndef __STATIC_INSTANCES_HPP__
#define __STATIC_INSTANCES_HPP__
#include <InputManager.hpp>
#include <Window.hpp>
#include <GraphicsEngine.hpp>
#include <IApp.hpp>
#include <ObjectManager.hpp>
#include <string>

class AppInst : public _ObjectManager<IApp> {
public:
	static void Init();
};

enum class IOType {
	Pluto
};

class IOInst : public _ObjectManager<InputManager> {
public:
	static void Init(IOType type = IOType::Pluto);
};

enum class WindowType {
	Luna
};

class WindowInst : public _ObjectManager<Window> {
public:
	static void Init(
		int width, int height, InputManager* ioMan, const char* name,
		WindowType type = WindowType::Luna
	);
};

enum class RendererType {
	Terra,
	Gaia
};

class RendererInst : public _ObjectManager<GraphicsEngine> {
public:
	static void Init(
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		RendererType type = RendererType::Gaia,
		std::uint8_t bufferCount = 2u
	);

	static void SetAPI(RendererType type) noexcept;
};

#endif
