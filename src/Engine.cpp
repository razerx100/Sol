#include <Engine.hpp>

Engine::Engine() {
	// Initialize the members
}

int Engine::Run() {
	while (true) {
		m_window->Update();
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
