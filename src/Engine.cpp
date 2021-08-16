#include <Engine.hpp>

Engine::Engine() {
	// Initialize the members
}

int Engine::Run() {
	while (true) {

		if (auto ecode = m_window->Update())
			return *ecode;

		m_pipelineManager->Render();
	}
}

Window* Engine::GetWindowRef() const noexcept {
	return m_window.get();
}

PipelineManager* Engine::GetPipelineManagerRef() const noexcept {
	return m_pipelineManager.get();
}

App* Engine::GetAppRef() const noexcept {
	return m_app.get();
}
