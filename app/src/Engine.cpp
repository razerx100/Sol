#include <Engine.hpp>

#include <Sol.hpp>
#include <DirectXMath.h>

Engine::Engine() : m_appName("Sol")
{

	m_objectManager.CreateObject(Sol::configManager, { L"config.ini" }, 0u);
	Sol::configManager->ReadConfigFile();

	Sol::InitThreadPool(m_objectManager, 8u);

	Sol::InitIoMan(m_objectManager, Sol::configManager->GeIOName());
	Sol::ioMan->AddGamepadSupport(1u);

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

	const RenderEngineType engineType = Sol::configManager->GetRenderEngineType();

	Sol::InitRenderer(
		m_objectManager,
		m_appName.c_str(),
		Sol::window->GetWindowHandle(),
		Sol::window->GetModuleInstance(),
		1920u, 1080u,
		Sol::threadPool,
		engineType,
		Sol::configManager->GetRendererName()
	);
	Sol::renderer->SetShaderPath(L"resources/shaders/");
	Sol::renderer->SetBackgroundColour({ 0.01f, 0.01f, 0.01f, 0.01f });

	Sol::window->SetRenderer(Sol::renderer);

	m_objectManager.CreateObject(Sol::app, 1u);

	AMods::InitAppModules(m_objectManager);
	/*
	TextureTool::AddDefaultTexture();

	bool processMeshlets = engineType == RenderEngineType::MeshDraw;
	m_objectManager.CreateObject(Sol::modelProcessor, { processMeshlets }, 0u);
	m_objectManager.CreateObject(Sol::modelContainer, 0u);
	m_objectManager.CreateObject(Sol::textureAtlas, 0u);

	Sol::textureAtlas->CreateAtlas();

	Sol::modelProcessor->MoveData();

	if (Sol::textureAtlas->DoesTextureExist()) {
		const size_t atlasIndex = Sol::renderer->AddTexture(
			Sol::textureAtlas->MoveTexture(),
			Sol::textureAtlas->GetWidth(), Sol::textureAtlas->GetHeight()
		);
		Sol::textureAtlas->SetTextureIndex(atlasIndex);
	}

	Sol::app->SetResources();
	Sol::modelContainer->SetResources();

	Sol::modelProcessor.reset();
	*/
}

int Engine::Run()
{
	int errorCode = -1;

	float accumulatedElapsedTime = 0;

	while (true)
	{
		AMods::frameTime->GetTimer().SetTimer();

		if (auto ecode = Sol::window->Update(); ecode)
		{
			errorCode = *ecode;
			break;
		}

		if(!Sol::window->IsMinimized())
		{
			float deltaTime = AMods::frameTime->GetDeltaTime();
			float updateDelta = AMods::frameTime->GetGraphicsUpdateDelta();

			if (accumulatedElapsedTime >= updateDelta)
			{
				while (accumulatedElapsedTime >= updateDelta)
				{
					//Sol::modelContainer->PhysicsUpdate();
					Sol::app->PhysicsUpdate();
					accumulatedElapsedTime -= updateDelta;
				}

				accumulatedElapsedTime = 0;
			}
			else
				accumulatedElapsedTime += deltaTime;

			Sol::window->UpdateIndependentInputs();
			Sol::app->PerFrameUpdate();
			Sol::renderer->Render();
		}

		AMods::frameTime->EndTimer();

		if (AMods::frameTime->HasASecondPassed())
		{
			static std::string rendererName = Sol::configManager->GetRendererName();
			Sol::window->SetTitle(
				m_appName + " Renderer : " + rendererName
				+ " " + std::to_string(AMods::frameTime->GetFrameCount()) + "fps"
			);
			AMods::frameTime->ResetFrameCount();
		}
	}

	Sol::renderer->WaitForGPUToFinish();

	return errorCode;
}
