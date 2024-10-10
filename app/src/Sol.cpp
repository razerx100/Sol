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

	m_inputManager->SubscribeToEvent(InputEvent::Fullscreen, &FullscreenCallback, this);
	m_inputManager->SubscribeToEvent(InputEvent::Resize, &ResizeCallback, m_renderer.get());

	// Window
	m_window->SetWindowIcon(L"resources/icon/Sol.ico");
	m_window->SetTitle(m_appName + " Renderer : " + m_configManager.GetRendererName());

	SetInputCallback(*m_window, m_inputManager.get(), m_configManager.GeIOName());

	// Renderer
	m_renderer->SetShaderPath(L"resources/shaders/");
	m_renderer->SetBackgroundColour({ 0.01f, 0.01f, 0.01f, 0.01f });

	// App
	m_app->Init();
}

std::unique_ptr<InputManager> Sol::CreateInputManager(const std::string& moduleName)
{
	std::unique_ptr<InputManager> inputManager{};

	if (moduleName == "Pluto")
		inputManager = CreatePlutoInstance();

	return inputManager;
}

std::unique_ptr<Window> Sol::CreateWindow(
	const std::string& moduleName, const std::string& appName, std::uint32_t width, std::uint32_t height
) {
	std::unique_ptr<Window> window{};

	if (moduleName == "Luna")
		window = CreateLunaInstance(width, height, appName.c_str());

	return window;
}

std::unique_ptr<Renderer> Sol::CreateRenderer(
	const std::string& moduleName, const std::string& appName,
	std::uint32_t width, std::uint32_t height, std::uint32_t frameCount,
	std::shared_ptr<ThreadPool> threadPool,
	void* windowHandle, void* moduleHandle, RenderEngineType engineType
) {
	std::unique_ptr<Renderer> renderer{};

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
	std::int32_t errorCode       = -1;
	float accumulatedElapsedTime = 0;

	while (true)
	{
		m_frameTime.GetTimer().SetTimer();

		const std::int32_t exitCode = m_window->Update();

		if (!exitCode)
		{
			errorCode = exitCode;

			break;
		}

		if (!m_window->IsMinimised())
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

			m_inputManager->UpdateIndependentInputs();
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

void Sol::SetInputCallback(
	Window& window, InputManager* inputManager, const std::string& ioModuleName
) {
	if (ioModuleName == "Pluto")
		window.AddInputCallback(&Win32InputCallbackProxy, inputManager);
}

void Sol::Win32InputCallbackProxy(
	void* hwnd, std::uint32_t message, std::uint64_t wParam, std::uint64_t lParam,
	void* extraData
) {
	auto inputManager = static_cast<InputManager*>(extraData);

	inputManager->InputCallback(hwnd, message, wParam, lParam);
}

void Sol::ResizeCallback(void* callbackData, void* extraData)
{
	auto resizeData = static_cast<ResizeData*>(callbackData);
	auto renderer   = static_cast<Renderer*>(extraData);

	renderer->Resize(resizeData->width, resizeData->height);
}

void Sol::FullscreenCallback([[maybe_unused]]void* callbackData, void* extraData)
{
	auto sol = static_cast<Sol*>(extraData);

	const Renderer::Resolution resolution = sol->m_renderer->GetFirstDisplayCoordinates();

	sol->m_window->ToggleFullscreen(resolution.width, resolution.height);
}
