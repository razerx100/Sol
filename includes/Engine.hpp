#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__
#include <Window.hpp>
#include <App.hpp>
#include <memory>

class Engine {
public:
	Engine();

	int Run();

private:
	Window* m_pWindowRef;
	App* m_pAppRef;
};
#endif
