#ifndef SOL_HPP_
#define SOL_HPP_
#include <memory>
#include <App.hpp>
#include <InputManager.hpp>
#include <Window.hpp>
#include <IGraphicsEngine.hpp>
#include <ModelContainer.hpp>
#include <UploadBuffer.hpp>
#include <TextureAtlas.hpp>

namespace Sol {
	// Variables
	extern std::unique_ptr<App> app;
	extern std::shared_ptr<InputManager> ioMan;
	extern std::unique_ptr<Window> window;
	extern std::shared_ptr<GraphicsEngine> renderer;
	extern std::unique_ptr<ModelContainer> modelContainer;
	extern std::unique_ptr<UploadBuffer> uploadBuffer;
	extern std::unique_ptr<TextureAtlas> textureAtlas;

	// Enums
	enum class IoType {
		Pluto
	};

	enum class WindowType {
		Luna
	};

	enum class RendererType {
		Terra,
		Gaia
	};

	// Initialization functions
	void InitApp();
	void InitIoMan(IoType type = IoType::Pluto);
	void InitWindow(
		std::uint32_t width, std::uint32_t height, const char* name,
		WindowType type = WindowType::Luna
	);
	void InitRenderer(
		const char* appName,
		void* windowHandle,
		void* moduleHandle,
		std::uint32_t width, std::uint32_t height,
		RendererType type = RendererType::Gaia,
		std::uint8_t bufferCount = 2u
	);
	void InitModelContainer();
	void InitUploadBuffer();
	void InitTextureAtlas();
}

#endif
