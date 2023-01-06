#include <Engine.hpp>
#include <VenusInstance.hpp>

#include <Sol.hpp>
#include <DirectXMath.h>

Engine::Engine()
	: m_appName("Sol") {

	m_objectManager.CreateObject(Sol::configManager, { L"config.ini" }, 0u);
	Sol::configManager->ReadConfigFile();

	Sol::InitThreadPool(m_objectManager, 8u);
	Sol::InitSharedData(m_objectManager);

	Sol::InitIoMan(m_objectManager, Sol::configManager->GeIOName());
	Sol::ioMan->AddDeviceSupport(DeviceType::Keyboard);
	Sol::ioMan->AddDeviceSupport(DeviceType::Mouse);
	Sol::ioMan->AddDeviceSupport(DeviceType::Gamepad);

	Sol::InitWindow(
		m_objectManager,
		1920u, 1080u, m_appName.c_str(),
		Sol::configManager->GetWindowName()
	);
	Sol::window->SetWindowIcon(L"resources/icon/Sol.ico");
	Sol::window->SetInputManager(Sol::ioMan);

	Sol::window->SetTitle(
		m_appName + " Renderer : " + Sol::configManager->GetRendererName()
	);

	Sol::InitRenderer(
		m_objectManager,
		m_appName.c_str(),
		Sol::window->GetWindowHandle(),
		Sol::window->GetModuleInstance(),
		1920u, 1080u,
		Sol::configManager->GetRendererName()
	);
	Sol::renderer->SetShaderPath(L"resources/shaders/");
	Sol::renderer->SetThreadPool(Sol::threadPool);
	Sol::renderer->SetSharedDataContainer(Sol::sharedData);
	Sol::renderer->SetBackgroundColour({ 0.01f, 0.01f, 0.01f, 0.01f });

	AMods::InitAppModules(m_objectManager);

	m_objectManager.CreateObject(Sol::modelProcessor, 0u);
	m_objectManager.CreateObject(Sol::modelContainer, 0u);
	m_objectManager.CreateObject(Sol::textureAtlas, 0u);

	Sol::window->SetRenderer(Sol::renderer);

	m_objectManager.CreateObject(Sol::app, 1u);

	Sol::textureAtlas->CreateAtlas();

	Sol::modelProcessor->Process();

	size_t textureIndex = Sol::renderer->RegisterResource(
		Sol::textureAtlas->MoveTexture(),
		Sol::textureAtlas->GetWidth(), Sol::textureAtlas->GetHeight()
	);

	Sol::renderer->ProcessData();

	Sol::app->SetResources();
	Sol::modelContainer->SetResources();

	Sol::modelProcessor.reset();
}

int Engine::Run() {
	int errorCode = -1;

	float accumulatedElapsedTime = 0;

	while (true) {
		AMods::frameTime->StartTimer();

		if (auto ecode = Sol::window->Update(); ecode) {
			errorCode = *ecode;
			break;
		}

		if(!Sol::window->IsMinimized()) {
			float deltaTime = AMods::frameTime->GetDeltaTime();
			float updateDelta = AMods::frameTime->GetGraphicsUpdateDelta();

			if (accumulatedElapsedTime >= updateDelta) {
				while (accumulatedElapsedTime >= updateDelta) {
					Sol::modelContainer->PhysicsUpdate();
					Sol::app->PhysicsUpdate();
					accumulatedElapsedTime -= updateDelta;
				}

				accumulatedElapsedTime = 0;
			}
			else
				accumulatedElapsedTime += deltaTime;

			Sol::window->UpdateIndependentInputs();
			Sol::app->PerFrameUpdate();
			Sol::renderer->Update();
			Sol::renderer->Render();
		}

		AMods::frameTime->EndTimer();

		if (AMods::frameTime->HasASecondPassed()) {
			static std::string rendererName = Sol::configManager->GetRendererName();
			Sol::window->SetTitle(
				m_appName + " Renderer : " + rendererName
				+ " " + std::to_string(AMods::frameTime->GetFrameCount()) + "fps"
			);
			AMods::frameTime->ResetFrameCount();
		}
	}

	WaitForAsyncTasks();

	return errorCode;
}

void Engine::WaitForAsyncTasks() {
	Sol::renderer->WaitForAsyncTasks();
}
