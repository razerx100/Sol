#include <Engine.hpp>
#include <Exception.hpp>
#include <InputManager.hpp>

Engine::Engine() {
	InitInputManagerInstance();
	GetInputManagerInstance()->AddDeviceSupport(DeviceType::KeyboardDev);
	GetInputManagerInstance()->AddDeviceSupport(DeviceType::MouseDev);
	InitWindowInstance(1920, 1080, "Sol");
	m_pWindowRef = GetWindowInstance();
	m_pWindowRef->SetWindowIcon("icon\\Sol.ico");

	InitGraphicsEngineInstance(m_pWindowRef->GetWindowHandle());
	m_pGraphicsRef = GetGraphicsEngineInstance();

	InitAppInstance();
	m_pAppRef = GetAppInstance();
}

Engine::~Engine() noexcept {
	CleanUpAppInstance();
	CleanUpGraphicsEngineInstance();
	CleanUpWindowInstance();
	CleanUpInputManagerInstance();
}

int Engine::Run() {
	while (true) {
		if (int ecode = m_pWindowRef->Update(); !ecode)
			return ecode;

		m_pAppRef->Update();
		m_pGraphicsRef->Render();
	}
}
