#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <App.hpp>
#include <InputManager.hpp>
#include <Window.hpp>
#include <ThreadPool.hpp>
#include <ConfigManager.hpp>
#include <TimeManager.hpp>

#include <Renderer.hpp>
#include <TextureAtlas.hpp>
#include <CameraManagerSol.hpp>
#include <ExtensionManager.hpp>

class Sol
{
public:
	Sol(const std::string& appName);

	int Run();

private:
	[[nodiscard]]
	static std::unique_ptr<InputManager> CreateInputManager(const std::string& moduleName);
	[[nodiscard]]
	static std::unique_ptr<Window> CreateWindow(
		const std::string& moduleName, const std::string& appName,
		std::uint32_t width, std::uint32_t height
	);
	[[nodiscard]]
	static std::unique_ptr<Renderer> CreateRenderer(
		const std::string& moduleName, const std::string& appName,
		std::uint32_t width, std::uint32_t height, std::uint32_t frameCount,
		std::shared_ptr<ThreadPool> threadPool,
		void* windowHandle, void* moduleHandle, RenderEngineType engineType
	);

	static void Win32InputCallbackProxy(
		void* hwnd, std::uint32_t message, std::uint64_t wParam, std::uint64_t lParam,
		void* extraData
	);
	static void SetInputCallback(
		Window& window, InputManager* inputManager, const std::string& ioModuleName
	);

	static void ResizeCallback(void* callbackData, void* extraData);
	static void FullscreenCallback(void* callbackData, void* extraData);

private:
	static constexpr std::uint32_t s_width      = 1920u;
	static constexpr std::uint32_t s_height     = 1080u;
	static constexpr std::uint32_t s_frameCount = 2u;

private:
	std::string                   m_appName;
	ConfigManager                 m_configManager;
	FrameTime                     m_frameTime;
	std::shared_ptr<ThreadPool>   m_threadPool;
	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<Window>       m_window;
	std::unique_ptr<Renderer>     m_renderer;
	ExtensionManager              m_extensionManager;
	std::unique_ptr<App>          m_app;

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
		m_extensionManager{ std::move(other.m_extensionManager) },
		m_app{ std::move(other.m_app) }
	{}
	Sol& operator=(Sol&& other) noexcept
	{
		m_appName          = std::move(other.m_appName);
		m_configManager    = std::move(other.m_configManager);
		m_frameTime        = std::move(other.m_frameTime);
		m_threadPool       = std::move(other.m_threadPool);
		m_inputManager     = std::move(other.m_inputManager);
		m_window           = std::move(other.m_window);
		m_renderer         = std::move(other.m_renderer);
		m_extensionManager = std::move(other.m_extensionManager);
		m_app              = std::move(other.m_app);

		return *this;
	}
};
#endif
