#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <vector>

#include <Sol.hpp>

namespace AMods {
	std::unique_ptr<FrameTime> frameTime;
	std::unique_ptr<CameraManagerSol> cameraManager;

	void InitAppModules(ObjectManager& om) {
		om.CreateObject(frameTime, 0u);
		om.CreateObject(cameraManager, 0u);
	}
}

namespace Sol {
	// Variables
	std::unique_ptr<App> app;
	std::shared_ptr<InputManager> ioMan;
	std::unique_ptr<Window> window;
	std::shared_ptr<Renderer> renderer;
	std::unique_ptr<ModelProcessor> modelProcessor;
	std::unique_ptr<ModelContainer> modelContainer;
	std::unique_ptr<TextureAtlas> textureAtlas;
	std::shared_ptr<ThreadPool> threadPool;
	std::unique_ptr<ConfigManager> configManager;
	std::shared_ptr<ISharedDataContainer> sharedData;

	// Functions
	void InitIoMan(ObjectManager& om, std::string moduleName) {
		if (moduleName == "Pluto")
			om.CreateObject(ioMan, CreatePlutoInstance(), 3u);
	}

	void InitWindow(
		ObjectManager& om, std::uint32_t width, std::uint32_t height, const char* name,
		std::string moduleName
	) {
		if (moduleName == "Luna")
			om.CreateObject(window, CreateLunaInstance(width, height, name), 3u);
	}

	void InitRenderer(
		ObjectManager& om,
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		std::shared_ptr<ThreadPool> threadPooll,
		RenderEngineType engineType,
		std::string moduleName,
		std::uint8_t bufferCount
	) {
		if (moduleName == "Gaia")
			om.CreateObject(
				renderer, CreateGaiaInstance(
					appName,
					windowHandle,
					width, height,
					std::move(threadPooll),
					engineType,
					bufferCount
				), 2u
			);
		else if (moduleName == "Terra")
			om.CreateObject(
				renderer, CreateTerraInstance(
					appName,
					windowHandle, moduleHandle,
					width, height,
					std::move(threadPooll),
					engineType,
					bufferCount
				), 2u
			);
	}

	void InitThreadPool(ObjectManager& om, size_t threadCount) {
		om.CreateObject(threadPool, std::make_shared<ThreadPool>(threadCount), 3u);
	}

	void InitSharedData(ObjectManager& om) {
		om.CreateObject<SharedDataContainer>(sharedData, 3u);
	}
}
