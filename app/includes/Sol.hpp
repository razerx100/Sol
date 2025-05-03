#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <vector>
#include <InputManager.hpp>
#include <ThreadPool.hpp>
#include <ConfigManager.hpp>
#include <TimeManager.hpp>

#include <RendererVK.hpp>
#include <RendererDx12.hpp>
#include <TextureAtlas.hpp>
#include <CameraManagerSol.hpp>
#include <ExtensionManager.hpp>
#include <RenderPassManager.hpp>

#ifdef SOL_WIN32
#include <WinWindow.hpp>
#include <VkSurfaceManagerWin32.hpp>
#include <VkDisplayManagerWin32.hpp>
#endif

namespace Sol
{
template<
	RendererModule rendererModule,
	WindowModule windowModule,
	InputModule inputModule,
	class rendererEngine_t,
	class App_t
>
class Sol
{
	template<WindowModule windowModule>
	struct WindowModuleConditional { using type = std::void_t<void>; };

#ifdef SOL_WIN32
	using SurfaceManager_t = Terra::SurfaceManagerWin32;
	using DisplayManager_t = Terra::DisplayManagerWin32;

	template<>
	struct WindowModuleConditional<WindowModule::Luna> { using type = Luna::WinWindow; };
#endif

	using RendererVK_t  = Terra::RendererVK<SurfaceManager_t, DisplayManager_t, rendererEngine_t>;

	using RendererD3D_t = Gaia::RendererDx12<rendererEngine_t>;

	template<RendererModule rendererModule>
	struct RenderPassImplConditional { using type = Terra::VkExternalRenderPass; };

	template<>
	struct RenderPassImplConditional<RendererModule::Gaia>
	{ using type = Gaia::D3DExternalRenderPass; };

	template<RendererModule rendererModule>
	struct RendererModuleConditional { using type = RendererVK_t; };

	template<>
	struct RendererModuleConditional<RendererModule::Gaia> { using type = RendererD3D_t; };

	template<InputModule inputModule>
	struct InputModuleConditional { using type = Pluto::InputManager; };

	using Renderer_t          = typename RendererModuleConditional<rendererModule>::type;

	using Window_t            = typename WindowModuleConditional<windowModule>::type;

	using InputManager_t      = typename InputModuleConditional<inputModule>::type;

	using RenderPassImpl_t    = typename RenderPassImplConditional<rendererModule>::type;

	using RenderPassManager_t = RenderPassManager<RenderPassImpl_t>;

	using ExtensionManager_t  = ExtensionManager<RenderPassImpl_t>;

public:
	Sol(const std::string& appName, ConfigManager&& configManager)
		: m_appName{ appName },
		m_configManager{ std::move(configManager) },
		m_frameTime{},
		m_threadPool{ std::make_shared<ThreadPool>( 8u ) },
		m_inputManager{ CreateInputManager() },
		m_window{ CreateWindowModule(s_width, s_height, appName) },
		m_renderer{
			CreateRenderer(
				appName, s_width, s_height, s_frameCount,
				m_threadPool, m_window.GetWindowHandle(), m_window.GetModuleInstance()
			)
		},
		m_extensionManager{},
		m_renderPassManager{ m_configManager.GetRenderEngineType() },
		m_app{ m_extensionManager, m_renderPassManager, s_frameCount }
	{
		// Input Manager
		m_inputManager.AddGamepadSupport(1u);

		m_inputManager.SubscribeToEvent(InputEvent::Fullscreen, &FullscreenCallback, this);

		m_inputManager.SubscribeToEvent(InputEvent::Resize, &ResizeCallback, this);

		// Window
		m_window.SetWindowIcon(L"resources/icon/Sol.ico");
		m_window.SetTitle(m_appName + " Renderer : " + m_configManager.GetRendererName());

		SetInputCallback(m_window, &m_inputManager);

		// Renderer
		m_renderer.SetShaderPath(L"resources/shaders/");

		m_renderPassManager.CreateResources(
			m_renderer.GetExternalResourceManager().GetResourceFactory()
		);

		m_extensionManager.SetBuffers(m_renderer);
		m_extensionManager.SetAllExtensions(m_renderer);

		// This function creates the descriptor layouts.
		m_renderer.FinaliseInitialisation();

		// Since we are binding the texture, it must be after the layouts have been created.
		// And before any other textures have been bound.
		AddDefaultTexture();

		m_renderPassManager.SetupRenderPassesFromRenderer(m_renderer);
		// Resize to create all the textures. The queues should be not running now.
		m_renderPassManager.Resize(m_renderer);

		// The descriptor layouts should be set with the FinaliseInitialisation function. So,
		// Create the fixed Descriptors here.
		m_extensionManager.SetFixedDescriptors(m_renderer);

		// App
		m_app.Init(m_renderer, m_extensionManager, m_renderPassManager);
	}

	int Run()
	{
		std::int32_t errorCode       = -1;
		float accumulatedElapsedTime = 0;

		while (true)
		{
			m_frameTime.GetTimer().SetTimer();

			const std::int32_t exitCode = m_window.Update();

			if (!exitCode)
			{
				errorCode = exitCode;

				break;
			}

			if (!m_window.IsMinimised())
			{
				float deltaTime   = m_frameTime.GetDeltaTime();
				float updateDelta = m_frameTime.GetGraphicsUpdateDelta();

				if (accumulatedElapsedTime >= updateDelta)
				{
					while (accumulatedElapsedTime >= updateDelta)
					{
						m_app.PhysicsUpdate(
							m_inputManager, m_renderer, m_extensionManager, m_renderPassManager
						);
						accumulatedElapsedTime -= updateDelta;
					}

					accumulatedElapsedTime = 0;
				}
				else
					accumulatedElapsedTime += deltaTime;

				m_inputManager.UpdateIndependentInputs();

				const size_t backBufferIndex = m_renderer.WaitForCurrentBackBuffer();

				m_renderer.Update(backBufferIndex);

				m_extensionManager.UpdateCPUData(backBufferIndex);

				m_renderer.Render(backBufferIndex);
			}

			m_frameTime.EndTimer();

			if (m_frameTime.HasASecondPassed())
			{
				static std::string rendererName = m_configManager.GetRendererName();

				m_window.SetTitle(
					m_appName + " Renderer : " + rendererName
					+ " " + std::to_string(m_frameTime.GetFrameCount()) + "fps"
				);
				m_frameTime.ResetFrameCount();
			}
		}

		m_renderer.WaitForGPUToFinish();

		return errorCode;
	}

private:
	[[nodiscard]]
	static InputManager_t CreateInputManager()
	{
		return InputManager_t{};
	}

	[[nodiscard]]
	static Window_t CreateWindowModule(
		std::uint32_t width, std::uint32_t height, const std::string& appName
	) {
		return Window_t{ width, height, appName.c_str() };
	}

	[[nodiscard]]
	static Renderer_t CreateRenderer(
		const std::string& appName, std::uint32_t width, std::uint32_t height,
		std::uint32_t frameCount, std::shared_ptr<ThreadPool> threadPool, void* windowHandle,
		void* moduleHandle
	) {
		if constexpr (rendererModule == RendererModule::Gaia)
			return Renderer_t
			{
				windowHandle, width, height, frameCount, std::move(threadPool)
			};
		else
			return Renderer_t
			{
				appName.c_str(), windowHandle, moduleHandle, width, height, frameCount,
				std::move(threadPool)
			};
	}

	static void Win32InputCallbackProxy(
		void* hwnd, std::uint32_t message, std::uint64_t wParam, std::uint64_t lParam,
		void* extraData
	) {
		auto inputManager = static_cast<InputManager_t*>(extraData);

		inputManager->InputCallback(hwnd, message, wParam, lParam);
	}

	static void SetInputCallback(Window_t& window, InputManager_t* inputManager)
	{
		if constexpr (inputModule == InputModule::Pluto)
#ifdef SOL_WIN32
			window.AddInputCallback(&Win32InputCallbackProxy, inputManager);
#endif
	}

	static void ResizeCallback(void* callbackData, void* extraData)
	{
		auto resizeData = static_cast<ResizeData*>(callbackData);
		auto sol        = static_cast<Sol*>(extraData);

		Renderer_t& renderer = sol->m_renderer;

		renderer.Resize(resizeData->width, resizeData->height);

		sol->m_renderPassManager.Resize(renderer);
	}

	static void FullscreenCallback([[maybe_unused]] void* callbackData, void* extraData)
	{
		auto sol = static_cast<Sol*>(extraData);

		const RendererType::Extent resolution = sol->m_renderer.GetFirstDisplayCoordinates();

		sol->m_window.ToggleFullscreen(resolution.width, resolution.height);
	}

	void AddDefaultTexture()
	{
		// The texture should be in RGBA
		struct Pixel
		{
			std::uint8_t r;
			std::uint8_t g;
			std::uint8_t b;
			std::uint8_t a;
		} pixel{ .r = 255u, .g = 255u, .b = 255u, . a = 26u };
		// If the default texture is used in an opaque mesh then the alpha value shouldn't matter.
		// And on a transparent object, we would probably want it to be translucent.

		STexture defaultTexture{};

		defaultTexture.data   = std::make_shared<Pixel>(pixel);
		defaultTexture.height = 1u;
		defaultTexture.width  = 1u;

		size_t defaultTexIndex  = m_renderer.AddTexture(std::move(defaultTexture));
		std::uint32_t bindIndex = m_renderer.BindTexture(defaultTexIndex);

		SetDefaultTextureDetails(static_cast<std::uint32_t>(defaultTexIndex), bindIndex);
	}

	void ResetCallbacks() noexcept
	{
		// Remove all the old callbacks.
		m_inputManager.UnsubscribeAllCallbacks(InputEvent::Fullscreen);

		m_inputManager.UnsubscribeAllCallbacks(InputEvent::Resize);

		m_window.RemoveAllCallbacks();

		// Add the new ones.
		m_inputManager.SubscribeToEvent(InputEvent::Fullscreen, &FullscreenCallback, this);

		m_inputManager.SubscribeToEvent(InputEvent::Resize, &ResizeCallback, this);

		SetInputCallback(m_window, &m_inputManager);
	}

private:
	// Default resolution
	static constexpr std::uint32_t s_width      = 1920u;
	static constexpr std::uint32_t s_height     = 1080u;
	static constexpr std::uint32_t s_frameCount = 2u;

private:
	std::string                 m_appName;
	ConfigManager               m_configManager;
	FrameTime                   m_frameTime;
	std::shared_ptr<ThreadPool> m_threadPool;
	InputManager_t              m_inputManager;
	Window_t                    m_window;
	Renderer_t                  m_renderer;
	RenderPassManager_t         m_renderPassManager;
	ExtensionManager_t          m_extensionManager;
	App_t                       m_app;

public:
	Sol(const Sol&) = delete;
	Sol& operator=(const Sol&) = delete;

	Sol(Sol&& other) noexcept
		: m_appName{ std::move(other.m_appName) },
		m_configManager{ std::move(other.m_configManager) },
		m_frameTime{ std::move(other.m_frameTime) },
		m_threadPool{ std::move(other.m_threadPool) },
		m_inputManager{ std::move(other.m_inputManager) },
		m_window{ std::move(other.m_window) },
		m_renderer{ std::move(other.m_renderer) },
		m_renderPassManager{ std::move(other.m_renderPassManager) },
		m_extensionManager{ std::move(other.m_extensionManager) },
		m_app{ std::move(other.m_app) }
	{
		// Need to reset the callbacks after a move, as they would have the pointers
		// to the old object, which should be destroyed after this function call.
		ResetCallbacks();
	}
	Sol& operator=(Sol&& other) noexcept
	{
		m_appName           = std::move(other.m_appName);
		m_configManager     = std::move(other.m_configManager);
		m_frameTime         = std::move(other.m_frameTime);
		m_threadPool        = std::move(other.m_threadPool);
		m_inputManager      = std::move(other.m_inputManager);
		m_window            = std::move(other.m_window);
		m_renderer          = std::move(other.m_renderer);
		m_renderPassManager = std::move(other.m_renderPassManager);
		m_extensionManager  = std::move(other.m_extensionManager);
		m_app               = std::move(other.m_app);

		// Need to reset the callbacks after a move, as they would have the pointers
		// to the old object, which should be destroyed after this function call.
		ResetCallbacks();

		return *this;
	}
};
}
#endif
