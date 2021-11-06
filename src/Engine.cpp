#include <Engine.hpp>
#include <Exception.hpp>
#include <InputManager.hpp>

Engine::Engine() {
	InitInputManagerInstance();
	GetInputManagerInstance()->AddDeviceSupport(DeviceType::Keyboard);
	GetInputManagerInstance()->AddDeviceSupport(DeviceType::Mouse);
	GetInputManagerInstance()->AddDeviceSupport(DeviceType::Gamepad);
	InitWindowInstance(1920, 1080, "Sol");
	m_pWindowRef = GetWindowInstance();
	m_pWindowRef->SetWindowIcon("icon\\Sol.ico");

	InitGraphicsEngineInstance(
		m_pWindowRef->GetWindowHandle(),
		m_pWindowRef->GetModuleInstance(),
		1920u, 1080u
	);
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
