#include <Engine.hpp>
#include <InputManager.hpp>
#include <Exception.hpp>
#include <Luna/WinWindow.hpp>
#include <GaiaX/PipelineManagerDx12.hpp>

Engine::Engine() {
	InputManager inputManager;

	Window::Init(std::make_unique<WinWindow>(1920, 1080, "Sol"));
	m_pWindowRef = Window::GetRef();
	m_pWindowRef->SetWindowIcon("icon\\Sol.ico");

	PipelineManager::Init(
		std::make_unique<PipelineManagerDx12>(m_pWindowRef->GetWindowHandle())
	);
	m_pPipelineManagerRef = PipelineManager::GetRef();

	App::Init();
	m_pAppRef = App::GetRef();
}

int Engine::Run() {
	while (true) {
		if (auto ecode = m_pWindowRef->Update())
			return *ecode;

		m_pAppRef->Update();
		m_pPipelineManagerRef->Render();
	}
}
