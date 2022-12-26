#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <VenusInstance.hpp>
#include <vector>

#include <Sol.hpp>

namespace AMods {
	std::unique_ptr<FrameTime> frameTime;
	std::unique_ptr<CameraManagerSol> cameraManager;

	void InitAppModules() {
		Sol::objectManager.CreateObject(frameTime, 0u);
		Sol::objectManager.CreateObject(cameraManager, 0u);
	}
}

namespace Sol {
	// Variables
	ObjectManager objectManager;
	std::unique_ptr<App> app;
	std::shared_ptr<InputManager> ioMan;
	std::unique_ptr<Window> window;
	std::shared_ptr<Renderer> renderer;
	std::unique_ptr<ModelContainer> modelContainer;
	std::unique_ptr<TextureAtlas> textureAtlas;
	std::shared_ptr<IThreadPool> threadPool;
	std::unique_ptr<ConfigManager> configManager;
	std::shared_ptr<ISharedDataContainer> sharedData;

	// Functions
	void InitIoMan(std::string moduleName) {
		if (moduleName == "Pluto")
			objectManager.CreateObject(ioMan, CreatePlutoInstance(), 3u);
	}

	void InitWindow(
		std::uint32_t width, std::uint32_t height, const char* name,
		std::string moduleName
	) {
		if (moduleName == "Luna")
			objectManager.CreateObject(window, CreateLunaInstance(width, height, name), 3u);
	}

	void InitRenderer(
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		std::string moduleName,
		std::uint8_t bufferCount
	) {
		if (moduleName == "Gaia")
			objectManager.CreateObject(
				renderer, CreateGaiaInstance(
					appName,
					windowHandle,
					width, height,
					bufferCount
				), 2u
			);
		else if (moduleName == "Terra")
			objectManager.CreateObject(
				renderer, CreateTerraInstance(
					appName,
					windowHandle, moduleHandle,
					width, height,
					bufferCount
				), 2u
			);
	}

	void InitThreadPool(size_t threadCount) {
		objectManager.CreateObject(threadPool, CreateVenusInstance(threadCount), 3u);
	}

	void InitSharedData() {
		objectManager.CreateObject<SharedDataContainer>(sharedData, 3u);
	}
}
