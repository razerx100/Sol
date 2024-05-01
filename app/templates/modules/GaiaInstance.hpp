#ifndef GAIA_INSTANCE_HPP_
#define GAIA_INSTANCE_HPP_
#include <Renderer.hpp>
#include <IThreadPool.hpp>
#include <ISharedDataContainer.hpp>

#ifdef BUILD_GAIAX
#define GAIAX_DLL __declspec(dllexport)
#else
#define GAIAX_DLL __declspec(dllimport)
#endif

GAIAX_DLL Renderer* __cdecl CreateGaiaInstance(
	const char* appName,
	void* windowHandle,
	std::uint32_t width, std::uint32_t height,
	IThreadPool& threadPool, ISharedDataContainer& sharedContainer,
	RenderEngineType engineType, std::uint32_t bufferCount = 2u
);
#endif
