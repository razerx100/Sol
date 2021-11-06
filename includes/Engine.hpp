#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__
#include <Window.hpp>
#include <App.hpp>
#include <GraphicsEngine.hpp>
#include <string>

class Engine {
public:
	Engine();
	~Engine() noexcept;

	int Run();

private:
	Window* m_pWindowRef;
	GraphicsEngine* m_pGraphicsRef;
	App* m_pAppRef;

	const std::string m_appName;
};
#endif
