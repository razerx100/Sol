#ifndef __STATIC_INSTANCES_HPP__
#define __STATIC_INSTANCES_HPP__
#include <InputManager.hpp>
#include <Window.hpp>
#include <GraphicsEngine.hpp>
#include <IApp.hpp>
#include <ObjectManager.hpp>

class AppInst : public _ObjectManager<IApp> {
public:
	static void Init() noexcept;
};

enum class IOType {
	Pluto
};

class IOInst : public _ObjectManager<InputManager> {
public:
	static void Init(IOType type = IOType::Pluto) noexcept;
};

enum class WindowType {
	Luna
};

class WindowInst : public _ObjectManager<Window> {
public:
	static void Init(
		int width, int height, InputManager* ioMan, const char* name,
		WindowType type = WindowType::Luna
	) noexcept;
};

#endif
