#include <Engine.hpp>
#include <VenusInstance.hpp>

Engine::Engine()
	: m_appName("Sol") {

	CreateVenusInstance(8u);

	Sol::InitIoMan();
	Sol::ioMan->AddDeviceSupport(DeviceType::Keyboard);
	Sol::ioMan->AddDeviceSupport(DeviceType::Mouse);
	Sol::ioMan->AddDeviceSupport(DeviceType::Gamepad);

	Sol::InitWindow(1920u, 1080u,  m_appName.c_str());
	Sol::window->SetWindowIcon("resources/icon/Sol.ico");
	Sol::window->SetInputManager(Sol::ioMan);

	Sol::InitRenderer(
		m_appName.c_str(),
		Sol::window->GetWindowHandle(),
		Sol::window->GetModuleInstance(),
		1920u, 1080u
	);
	Sol::renderer->SetShaderPath("resources/shaders/");
	Sol::renderer->InitResourceBasedObjects();

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
	Sol::textureAtlas.reset();
	Sol::app.reset();
	Sol::renderer.reset();
	Sol::modelContainer.reset();
	Sol::window.reset();
	Sol::ioMan.reset();
	CleanUpVenusInstance();
}

int Engine::Run() {
	int errorCode = -1;

	while (true) {
		if (auto ecode = Sol::window->Update(); ecode) {
			errorCode = *ecode;
			break;
		}

		if(!Sol::window->IsMinimized()) {
			Sol::app->Update();
			Sol::renderer->Render();
		}
	}

	WaitForAsyncTasks();

	return errorCode;
}

void Engine::WaitForAsyncTasks() {
	Sol::renderer->WaitForAsyncTasks();
}
