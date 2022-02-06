#ifndef __STATIC_INSTANCES_HPP__
#define __STATIC_INSTANCES_HPP__
#include <InputManager.hpp>
#include <Window.hpp>
#include <GraphicsEngine.hpp>
#include <IApp.hpp>
#include <ObjectManager.hpp>
#include <IModelContainer.hpp>

class AppInst : public _ObjectManager<IApp, AppInst> {
public:
	static void Init();
};

enum class IOType {
	Pluto
};

class IOInst : public _ObjectManager<InputManager, IOInst> {
public:
	static void Init(IOType type = IOType::Pluto);
};

enum class WindowType {
	Luna
};

class WindowInst : public _ObjectManager<Window, WindowInst> {
public:
	static void Init(
		std::uint32_t width, std::uint32_t height, InputManager* ioMan, const char* name,
		WindowType type = WindowType::Luna
	);
};

enum class RendererType {
	Terra,
	Gaia
};

class RendererInst : public _ObjectManager<GraphicsEngine, RendererInst> {
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

class ModelContInst : public _ObjectManager<IModelContainer, ModelContInst> {
public:
	static void Init();
};
#endif
