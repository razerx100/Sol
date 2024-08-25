#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <vector>

#include <Sol.hpp>

Sol::Sol(const std::string& appName)
	: m_appName{ appName },
	m_configManager{ L"config.ini" }, m_frameTime{},
	m_threadPool{ std::make_shared<ThreadPool>( 8u ) },
	m_inputManager{ CreateInputManager(m_configManager.GeIOName()) },
	m_window{ CreateWindow(m_configManager.GetWindowName(), appName, s_width, s_height) },
	m_renderer{
		CreateRenderer(
			m_configManager.GetRendererName(), appName, s_width, s_height, s_frameCount,
			m_threadPool, m_window->GetWindowHandle(), m_window->GetModuleInstance(),
			m_configManager.GetRenderEngineType()
		)
	},
	m_app{
		std::make_unique<App>(m_renderer.get(), m_inputManager.get(), m_configManager.GetRenderEngineType())
	}
{
	// Input Manager
	m_inputManager->AddGamepadSupport(1u);

	// Window
	m_window->SetWindowIcon(L"resources/icon/Sol.ico");
	m_window->SetTitle(m_appName + " Renderer : " + m_configManager.GetRendererName());

	m_window->SetInputManager(m_inputManager);
	m_window->SetRenderer(m_renderer);

	// Renderer
	m_renderer->SetShaderPath(L"resources/shaders/");
	m_renderer->SetBackgroundColour({ 0.01f, 0.01f, 0.01f, 0.01f });

	// App
	m_app->Init();
}

std::shared_ptr<InputManager> Sol::CreateInputManager(const std::string& moduleName)
{
	std::shared_ptr<InputManager> inputManager{};

	if (moduleName == "Pluto")
		inputManager = std::shared_ptr<InputManager>{ CreatePlutoInstance() };

	return inputManager;
}

std::unique_ptr<Window> Sol::CreateWindow(
	const std::string& moduleName, const std::string& appName, std::uint32_t width, std::uint32_t height
) {
	std::unique_ptr<Window> window{};

	if (moduleName == "Luna")
		window = std::unique_ptr<Window>{ CreateLunaInstance(width, height, appName.c_str()) };

	return window;
}

std::shared_ptr<Renderer> Sol::CreateRenderer(
	const std::string& moduleName, const std::string& appName,
	std::uint32_t width, std::uint32_t height, std::uint32_t frameCount,
	std::shared_ptr<ThreadPool> threadPool,
	void* windowHandle, void* moduleHandle, RenderEngineType engineType
) {
	std::shared_ptr<Renderer> renderer{};

	if (moduleName == "Gaia")
		renderer = CreateGaiaInstance(
				appName.c_str(), windowHandle, width, height, std::move(threadPool), engineType, frameCount
			);
	else if (moduleName == "Terra")
		renderer = CreateTerraInstance(
				appName.c_str(), windowHandle, moduleHandle, width, height, std::move(threadPool),
				engineType, frameCount
			);

	return renderer;
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
