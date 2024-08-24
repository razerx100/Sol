#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <vector>

#include <Sol.hpp>

Sol::Sol(const std::string& appName)
	: m_appName{ appName },
	m_configManager{ L"config.ini" }, m_frameTime{}, m_threadPool{}, m_inputManager{},
	m_window{}, m_renderer{}, m_app{}
{
	m_configManager.ReadConfigFile();

	m_threadPool = std::make_shared<ThreadPool>( 8u );

	const std::uint32_t width  = 1920u;
	const std::uint32_t height = 1080u;

	InitInputManager();
	InitWindow(width, height);
	InitRenderer(width, height);
	InitApp();
}

void Sol::InitInputManager()
{
	if (m_configManager.GeIOName() == "Pluto")
		m_inputManager = std::shared_ptr<InputManager>{ CreatePlutoInstance() };

	m_inputManager->AddGamepadSupport(1u);
}

void Sol::InitWindow(std::uint32_t width, std::uint32_t height)
{
	if (m_configManager.GetWindowName() == "Luna")
		m_window = std::unique_ptr<Window>{ CreateLunaInstance(width, height, m_appName.c_str()) };

	m_window->SetWindowIcon(L"resources/icon/Sol.ico");
	m_window->SetInputManager(m_inputManager);

	m_window->SetTitle(m_appName + " Renderer : " + m_configManager.GetRendererName());
}

void Sol::InitRenderer(std::uint32_t width, std::uint32_t height)
{
	const std::uint32_t bufferCount = 2u;
	void* windowHandle              = m_window->GetWindowHandle();
	void* moduleHandle              = m_window->GetModuleInstance();
	RenderEngineType engineType     = m_configManager.GetRenderEngineType();
	const std::string moduleName    = m_configManager.GetRendererName();

	if (moduleName == "Gaia")
		m_renderer = std::shared_ptr<Renderer>{
			CreateGaiaInstance(
				m_appName.c_str(), windowHandle, width, height, m_threadPool, engineType, bufferCount
			)
		};
	else if (moduleName == "Terra")
		m_renderer = std::shared_ptr<Renderer>{
			CreateTerraInstance(
				m_appName.c_str(), windowHandle, moduleHandle,
				width, height, m_threadPool, engineType, bufferCount
			)
		};

	m_renderer->SetShaderPath(L"resources/shaders/");
	m_renderer->SetBackgroundColour({ 0.01f, 0.01f, 0.01f, 0.01f });

	m_window->SetRenderer(m_renderer);
}

void Sol::InitApp()
{
	m_app = std::make_unique<App>(
		m_renderer.get(), m_inputManager.get(), m_configManager.GetRenderEngineType()
	);
	m_app->Init();
}

int Sol::Run()
{
	int errorCode                = -1;
	float accumulatedElapsedTime = 0;

	while (true)
	{
		m_frameTime.GetTimer().SetTimer();

		if (auto ecode = m_window->Update(); ecode)
		{
			errorCode = *ecode;
			break;
		}

		if(!m_window->IsMinimized())
		{
			float deltaTime   = m_frameTime.GetDeltaTime();
			float updateDelta = m_frameTime.GetGraphicsUpdateDelta();

			if (accumulatedElapsedTime >= updateDelta)
			{
				while (accumulatedElapsedTime >= updateDelta)
				{
					m_app->PhysicsUpdate();
					accumulatedElapsedTime -= updateDelta;
				}

				accumulatedElapsedTime = 0;
			}
			else
				accumulatedElapsedTime += deltaTime;

			m_window->UpdateIndependentInputs();
			m_renderer->Render();
		}

		m_frameTime.EndTimer();

		if (m_frameTime.HasASecondPassed())
		{
			static std::string rendererName = m_configManager.GetRendererName();
			m_window->SetTitle(
				m_appName + " Renderer : " + rendererName
				+ " " + std::to_string(m_frameTime.GetFrameCount()) + "fps"
			);
			m_frameTime.ResetFrameCount();
		}
	}

	m_renderer->WaitForGPUToFinish();

	return errorCode;
}
