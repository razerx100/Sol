#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <App.hpp>
#include <InputManager.hpp>
#include <Window.hpp>
#include <Renderer.hpp>
#include <UploadBuffer.hpp>
#include <IThreadPool.hpp>
#include <ConfigManager.hpp>
#include <TimeManager.hpp>

#include <ModelContainer.hpp>
#include <TextureAtlas.hpp>

namespace Sol {
	// Variables
	extern std::unique_ptr<App> app;
	extern std::shared_ptr<InputManager> ioMan;
	extern std::unique_ptr<Window> window;
	extern std::shared_ptr<Renderer> renderer;
	extern std::unique_ptr<ModelContainer> modelContainer;
	extern std::unique_ptr<UploadBuffer> uploadBuffer;
	extern std::unique_ptr<TextureAtlas> textureAtlas;
	extern std::shared_ptr<IThreadPool> threadPool;
	extern std::unique_ptr<ConfigManager> configManager;
	extern std::unique_ptr<FrameTime> frameTime;

	// Initialization functions
	void InitApp();
	void InitIoMan(std::string moduleName = "Pluto");
	void InitWindow(
		std::uint32_t width, std::uint32_t height, const char* name,
		std::string moduleName = "Luna"
	);
	void InitRenderer(
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		std::string moduleName = "Gaia",
		std::uint8_t bufferCount = 2u
	);
	void InitModelContainer();
	void InitUploadBuffer();
	void InitTextureAtlas();
	void InitThreadPool(size_t threadCount);
	void InitConfigManager(const std::string& fileName);
	void InitFrameTime();
}
#endif