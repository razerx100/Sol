#include <Engine.hpp>
#include <Exception.hpp>
#include <IKeyboard.hpp>
#include <IMouse.hpp>

Engine::Engine() {
	InitKeyboardInstance();
	InitMouseInstance();
	InitWindowInstance(1920, 1080, "Sol");
	m_pWindowRef = GetWindowInstance();
	m_pWindowRef->SetWindowIcon("icon\\Sol.ico");

	InitGraphicsEngineInstance(m_pWindowRef->GetWindowHandle());
	m_pGraphicsRef = GetGraphicsEngineInstance();

	InitAppInstance();
	m_pAppRef = GetAppInstance();
}

Engine::~Engine() {
	CleanUpAppInstance();
	CleanUpGraphicsEngineInstance();
	CleanUpWindowInstance();
	CleanUpKeyboardInstance();
	CleanUpMouseInstance();
}

int Engine::Run() {
	while (true) {
		if (int ecode = m_pWindowRef->Update(); !ecode)
			return ecode;

		m_pAppRef->Update();
		m_pGraphicsRef->Render();
	}
}
