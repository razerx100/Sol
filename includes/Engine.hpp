#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__
#include <Window.hpp>
#include <App.hpp>
#include <GraphicsEngine.hpp>

class Engine {
public:
	Engine();
	~Engine();

	int Run();

private:
	Window* m_pWindowRef;
	GraphicsEngine* m_pGraphicsRef;
	App* m_pAppRef;
};
#endif
