#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__
#include <InstanceManager.hpp>
#include <string>

class Engine {
public:
	Engine();
	~Engine() noexcept;

	int Run();

	void WaitForAsyncTasks();

private:
	Window* m_pWindowRef;
	GraphicsEngine* m_pGraphicsRef;
	IApp* m_pAppRef;

	const std::string m_appName;
};
#endif
