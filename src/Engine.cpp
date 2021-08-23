#include <Engine.hpp>
#include <InputManager.hpp>
#include <WinWindow.hpp>
#include <resource.hpp>
#include <Exception.hpp>

std::unique_ptr<Window> Window::s_instance;
std::unique_ptr<PipelineManager> PipelineManager::s_instance;

Engine::Engine() {
	InputManager inputManager;
	Window::Init(std::make_unique<WinWindow>(1920, 1080, "Sol"));
	m_windowRef = Window::GetRef();
	m_windowRef->SetIcon(IDI_ICON);
	App::Init();
	m_appRef = App::GetRef();
}

int Engine::Run() {
	while (true) {
		if (auto ecode = m_windowRef->Update())
			return *ecode;

		m_appRef->Update();
		//m_pipelineManagerRef->Render();
	}
}
