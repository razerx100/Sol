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

class Sol
{
public:
	Sol(const std::string& appName);

	int Run();

private:
	void InitInputManager();
	void InitWindow(std::uint32_t width, std::uint32_t height);
	void InitRenderer(std::uint32_t width, std::uint32_t height);
	void InitApp();

private:
	std::string                   m_appName;
	ConfigManager                 m_configManager;
	FrameTime                     m_frameTime;
	std::shared_ptr<ThreadPool>   m_threadPool;
	std::shared_ptr<InputManager> m_inputManager;
	std::unique_ptr<Window>       m_window;
	std::shared_ptr<Renderer>     m_renderer;
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
		m_app{ std::move(other.m_app) }
	{}
	Sol& operator=(Sol&& other) noexcept
	{
		m_appName       = std::move(other.m_appName);
		m_configManager = std::move(other.m_configManager);
		m_frameTime     = std::move(other.m_frameTime);
		m_threadPool    = std::move(other.m_threadPool);
		m_inputManager  = std::move(other.m_inputManager);
		m_window        = std::move(other.m_window);
		m_renderer      = std::move(other.m_renderer);
		m_app           = std::move(other.m_app);

		return *this;
	}
};
#endif
