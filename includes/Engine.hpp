#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__
#include <Window.hpp>
#include <PipelineManager.hpp>
#include <App.hpp>
#include <memory>

class Engine {
public:
	Engine();

	int Run();

private:
	Window* m_windowRef;
	PipelineManager* m_pipelineManagerRef;
	App* m_appRef;
};
#endif
