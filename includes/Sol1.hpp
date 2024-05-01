#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <optional>
#include <App.hpp>
#include <InputManager.hpp>
#include <Window.hpp>
#include <IThreadPool.hpp>
#include <ConfigManager.hpp>
#include <TimeManager.hpp>
#include <ObjectManager.hpp>

#include <Renderer.hpp>
#include <ModelProcessor.hpp>
#include <ModelContainer.hpp>
#include <TextureAtlas.hpp>
#include <SharedDataContainer.hpp>
#include <CameraManagerSol.hpp>


class Sol
{
public:
	struct Args
	{
		std::optional<std::string_view> appName;
		std::optional<std::uint32_t> threadCount;
		std::optional<std::uint32_t> width;
		std::optional<std::uint32_t> height;
		std::optional<std::uint32_t> gamepadCount;
	};
public:
	Sol(const Args& arguments);

	inline ConfigManager& Config() noexcept { return *m_configManager; }
	inline IThreadPool& ThreadPool() noexcept { return *m_threadPool; }
	inline ISharedDataContainer& SharedData() noexcept { return *m_sharedData; }
	inline InputManager& IO() noexcept { return *m_io; }
	inline App& Application() noexcept { return *m_app; }

private:
	void InitIO(std::string_view moduleName);

private:
	std::string_view m_appName;
	ObjectManager m_objManager;

	std::unique_ptr<ConfigManager> m_configManager;
	std::unique_ptr<IThreadPool> m_threadPool;
	std::shared_ptr<ISharedDataContainer> m_sharedData;
	std::shared_ptr<InputManager> m_io;
	std::unique_ptr<App> m_app;
};
#endif