#include <Engine.hpp>
#include <Exception.hpp>
#include <VenusInstance.hpp>

Engine::Engine()
	: m_appName("Sol") {

	CreateVenusInstance(8u);

	IOInst::Init();
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Keyboard);
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Mouse);
	IOInst::GetRef()->AddDeviceSupport(DeviceType::Gamepad);
	WindowInst::Init(1920u, 1080u, IOInst::GetRef(), m_appName.c_str());
	m_pWindowRef = WindowInst::GetRef();
	m_pWindowRef->SetWindowIcon("resources/icon/Sol.ico");

	RendererInst::Init(
		m_appName.c_str(),
		m_pWindowRef->GetWindowHandle(),
		m_pWindowRef->GetModuleInstance(),
		1920u, 1080u
	);
	m_pGraphicsRef = RendererInst::GetRef();
	m_pGraphicsRef->SetShaderPath("resources/shaders/");
	m_pGraphicsRef->InitResourceBasedObjects();

	ModelContInst::Init();
	UploadBufferInst::Init();
	TexAtlasInst::Init();

	WindowInst::GetRef()->SetGraphicsEngineRef(m_pGraphicsRef);

	AppInst::Init();
	m_pAppRef = AppInst::GetRef();

	ITextureAtlas* texRef = TexAtlasInst::GetRef();
	texRef->CreateAtlas();

	ModelContInst::GetRef()->UpdateUVCoordinates();

	const std::vector<std::uint8_t>& texture = texRef->GetTexture();
	size_t textureIndex = m_pGraphicsRef->RegisterResource(
		texture.data(), texRef->GetRowPitch(), texRef->GetRows()
	);

	m_pGraphicsRef->ProcessData();

	UploadBufferInst::GetRef()->Release();
	ModelContInst::GetRef()->ClearModelBuffers();
	TexAtlasInst::CleanUp();
}

Engine::~Engine() noexcept {
	UploadBufferInst::CleanUp();
	AppInst::CleanUp();
	RendererInst::CleanUp();
	ModelContInst::CleanUp();
	WindowInst::CleanUp();
	IOInst::CleanUp();
	CleanUpVenusInstance();
}

int Engine::Run() {
	int errorCode = -1;

	while (true) {
		if (auto ecode = m_pWindowRef->Update(); ecode) {
			errorCode = *ecode;
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
