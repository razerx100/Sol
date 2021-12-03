#ifndef __GAIA_INSTANCE_HPP__
#define __GAIA_INSTANCE_HPP__
#include <IGraphicsEngine.hpp>

__declspec(dllimport) GraphicsEngine* __cdecl CreateGaiaInstance(
	const char* appName,
	void* windowHandle,
	std::uint32_t width, std::uint32_t height,
	std::uint8_t bufferCount = 2u
);
#endif
