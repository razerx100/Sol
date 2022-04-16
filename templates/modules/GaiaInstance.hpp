#ifndef GAIA_INSTANCE_HPP_
#define GAIA_INSTANCE_HPP_
#include <Renderer.hpp>

__declspec(dllimport) Renderer* __cdecl CreateGaiaInstance(
	const char* appName,
	void* windowHandle,
	std::uint32_t width, std::uint32_t height,
	std::uint32_t bufferCount = 2u
);
#endif
