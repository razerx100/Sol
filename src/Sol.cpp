#include <Sol.hpp>
#include <PlutoInstance.hpp>
#include <LunaInstance.hpp>
#include <GaiaInstance.hpp>
#include <TerraInstance.hpp>
#include <VenusInstance.hpp>
#include <ConfigManager.hpp>
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

	// Functions
	void InitApp() {
		app = std::make_unique<App>();
	}

	void InitIoMan(IoType type) {
		if (type == IoType::Pluto)
			ioMan = std::shared_ptr<InputManager>(CreatePlutoInstance());
	}

	void InitWindow(
		std::uint32_t width, std::uint32_t height, const char* name,
		WindowType type
	) {
		if (type == WindowType::Luna)
			window = std::unique_ptr<Window>(
				CreateLunaInstance(width, height, name)
				);
	}

	static const std::vector<const char*> RENDERERNAMES = {
		"Terra",
		"Gaia"
	};

	void InitRenderer(
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		RendererType type,
		std::uint8_t bufferCount
	) {
		type = ConfigManager::ReadType(
			"config.ini", "RendererType", type, RENDERERNAMES
		);

		if (type == RendererType::Gaia)
			renderer = std::shared_ptr<Renderer>(
				CreateGaiaInstance(
					appName,
					windowHandle,
					width, height,
					bufferCount
				)
			);
		else if (type == RendererType::Terra)
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
}
