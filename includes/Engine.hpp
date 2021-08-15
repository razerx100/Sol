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

	Window* GetWindowRef() const noexcept;
	PipelineManager* GetPipelineManagerRef() const noexcept;
	App* GetAppRef() const noexcept;

private:
	std::unique_ptr<Window> m_window;
	std::unique_ptr<PipelineManager> m_pipelineManager;
	std::unique_ptr<App> m_app;
};
#endif
