#include <Sol.hpp>
#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <VenusInstance.hpp>
#include <vector>

namespace Sol {
	// Variables
	std::unique_ptr<App> app;
	std::shared_ptr<InputManager> ioMan;
	std::unique_ptr<Window> window;
	std::shared_ptr<Renderer> renderer;
	std::unique_ptr<ModelContainer> modelContainer;
	std::unique_ptr<UploadBuffer> uploadBuffer;
	std::unique_ptr<TextureAtlas> textureAtlas;
	std::shared_ptr<IThreadPool> threadPool;
	std::unique_ptr<ConfigManager> configManager;

	// Functions
	void InitApp() {
		app = std::make_unique<App>();
	}

	void InitIoMan(std::string moduleName) {
		if (moduleName == "Pluto")
			ioMan = std::shared_ptr<InputManager>(CreatePlutoInstance());
	}

	void InitWindow(
		std::uint32_t width, std::uint32_t height, const char* name,
		std::string moduleName
	) {
		if (moduleName == "Luna")
			window = std::unique_ptr<Window>(
				CreateLunaInstance(width, height, name)
				);
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
			renderer = std::shared_ptr<Renderer>(
				CreateGaiaInstance(
					appName,
					windowHandle,
					width, height,
					bufferCount
				)
			);
		else if (moduleName == "Terra")
			renderer = std::shared_ptr<Renderer>(
				CreateTerraInstance(
					appName,
					windowHandle, moduleHandle,
					width, height,
					bufferCount
				)
			);
	}

	void InitModelContainer() {
		modelContainer = std::make_unique<ModelContainer>();
	}

	void InitUploadBuffer() {
		uploadBuffer = std::make_unique<UploadBuffer>();
	}

	void InitTextureAtlas() {
		textureAtlas = std::make_unique<TextureAtlas>();
	}

	void InitThreadPool(size_t threadCount) {
		threadPool = std::shared_ptr<IThreadPool>(CreateVenusInstance(threadCount));
	}

	void InitConfigManager(const std::string& fileName) {
		configManager = std::make_unique<ConfigManager>(fileName);
	}
}
