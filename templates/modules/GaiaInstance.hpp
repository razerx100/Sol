#ifndef GAIA_INSTANCE_HPP_
#define GAIA_INSTANCE_HPP_
#include <Renderer.hpp>

#ifdef BUILD_GAIAX
#define GAIAX_DLL __declspec(dllexport)
#else
#define GAIAX_DLL __declspec(dllimport)
#endif

GAIAX_DLL Renderer* __cdecl CreateGaiaInstance(
	const char* appName,
	void* windowHandle,
	std::uint32_t width, std::uint32_t height,
	RenderEngineType engineType, std::uint32_t bufferCount = 2u
);
#endif
