#include <Engine.hpp>
#include <VenusInstance.hpp>

#include <Sol.hpp>
#include <DirectXMath.h>
#include <AppModules.hpp>

Engine::Engine()
	: m_appName("Sol") {

	Sol::InitConfigManager("config.ini");
	Sol::configManager->ReadConfigFile();

	Sol::InitThreadPool(8u);
	Sol::InitSharedData();

	Sol::InitIoMan(Sol::configManager->GeIOName());
	Sol::ioMan->AddDeviceSupport(DeviceType::Keyboard);
	Sol::ioMan->AddDeviceSupport(DeviceType::Mouse);
	Sol::ioMan->AddDeviceSupport(DeviceType::Gamepad);

	Sol::InitWindow(
		1920u, 1080u,
		m_appName.c_str(),
		Sol::configManager->GetWindowName()
	);
	Sol::window->SetWindowIcon("resources/icon/Sol.ico");
	Sol::window->SetInputManager(Sol::ioMan);

	Sol::window->SetTitle(
		m_appName + " Renderer : " + Sol::configManager->GetRendererName()
	);

	Sol::InitRenderer(
		m_appName.c_str(),
		Sol::window->GetWindowHandle(),
		Sol::window->GetModuleInstance(),
		1920u, 1080u,
		Sol::configManager->GetRendererName()
	);
	Sol::renderer->SetShaderPath("resources/shaders/");
	Sol::renderer->InitResourceBasedObjects();
	Sol::renderer->SetThreadPool(Sol::threadPool);
	Sol::renderer->SetSharedDataContainer(Sol::sharedData);

	AMods::InitAppModules();

	Sol::InitModelContainer();
	Sol::InitUploadBuffer();
	Sol::InitTextureAtlas();

	Sol::window->SetRenderer(Sol::renderer);

	Sol::InitApp();

	Sol::textureAtlas->CreateAtlas();

	Sol::modelContainer->UpdateUVCoordinates();

	const std::vector<std::uint8_t>& texture = Sol::textureAtlas->GetTexture();
	size_t textureIndex = Sol::renderer->RegisterResource(
		texture.data(),
		Sol::textureAtlas->GetWidth(), Sol::textureAtlas->GetHeight(),
		Sol::textureAtlas-> GetPixelSizeInBytes()
	);

	Sol::renderer->ProcessData();

	Sol::app->SetResources();

	Sol::textureAtlas->CleanUpBuffer();

	Sol::uploadBuffer.reset();
	Sol::modelContainer->ClearModelBuffers();
}

Engine::~Engine() noexcept {
	AMods::ResetAppModules();
	Sol::configManager.reset();
	Sol::textureAtlas.reset();
	Sol::app.reset();
	Sol::renderer.reset();
	Sol::sharedData.reset();
	Sol::modelContainer.reset();
	Sol::window.reset();
	Sol::ioMan.reset();
	Sol::threadPool.reset();
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
					Sol::app->PhysicsUpdate();
					accumulatedElapsedTime -= updateDelta;
				}

				accumulatedElapsedTime = 0;
			}
			else
				accumulatedElapsedTime += deltaTime;

			Sol::app->PerFrameUpdate();
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
