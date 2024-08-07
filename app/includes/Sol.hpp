#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <App.hpp>
#include <InputManager.hpp>
#include <Window.hpp>
#include <ThreadPool.hpp>
#include <ConfigManager.hpp>
#include <TimeManager.hpp>
#include <ObjectManager.hpp>

#include <Renderer.hpp>
#include <ModelProcessor.hpp>
#include <TextureAtlas.hpp>
#include <CameraManagerSol.hpp>

namespace AMods {
	extern std::unique_ptr<FrameTime> frameTime;

	void InitAppModules(ObjectManager& om);
}

namespace Sol {
	// Variables
	extern std::unique_ptr<App> app;
	extern std::shared_ptr<InputManager> ioMan;
	extern std::unique_ptr<Window> window;
	extern std::shared_ptr<Renderer> renderer;
	//extern std::unique_ptr<ModelProcessor> modelProcessor;
	//extern std::unique_ptr<ModelContainer> modelContainer;
	extern std::unique_ptr<TextureAtlas> textureAtlas;
	extern std::shared_ptr<ThreadPool> threadPool;
	extern std::unique_ptr<ConfigManager> configManager;

	// Initialization functions
	void InitIoMan(ObjectManager& om, std::string moduleName = "Pluto");
	void InitWindow(
		ObjectManager& om, std::uint32_t width, std::uint32_t height, const char* name,
		std::string moduleName = "Luna"
	);
	void InitRenderer(
		ObjectManager& om,
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		std::shared_ptr<ThreadPool> threadPooll,
		RenderEngineType engineType,
		std::string moduleName = "Gaia",
		std::uint8_t bufferCount = 2u
	);
	void InitThreadPool(ObjectManager& om, size_t threadCount);
}
#endif
