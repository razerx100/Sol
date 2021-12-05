#include <Engine.hpp>
#include <Exception.hpp>

Engine::Engine()
	: m_appName("Sol") {
	IOInst::Init();
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Keyboard);
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Mouse);
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Gamepad);
	WindowInst::Init(1920, 1080, IOInst::GetRef(), m_appName.c_str());
	m_pWindowRef = WindowInst::GetRef();
	m_pWindowRef->SetWindowIcon("icon\\Sol.ico");

	RendererInst::Init(
		m_appName.c_str(),
		m_pWindowRef->GetWindowHandle(),
		m_pWindowRef->GetModuleInstance(),
		1920u, 1080u
	);
	m_pGraphicsRef = RendererInst::GetRef();
	WindowInst::GetRef()->SetGraphicsEngineRef(m_pGraphicsRef);

	AppInst::Init();
	m_pAppRef = AppInst::GetRef();
}

Engine::~Engine() noexcept {
	AppInst::CleanUp();
	RendererInst::CleanUp();
	WindowInst::CleanUp();
	IOInst::CleanUp();
}

int Engine::Run() {
	int errorCode = -1;
	while (true) {
		if (int ecode = m_pWindowRef->Update(); !ecode) {
			errorCode = ecode;
			break;
		}

		if(!m_pWindowRef->IsMinimized()) {
			m_pAppRef->Update();
			m_pGraphicsRef->Render();
		}
	}

	WaitForAsyncTasks();

	return errorCode;
}

void Engine::WaitForAsyncTasks() {
	m_pGraphicsRef->WaitForAsyncTasks();
}
