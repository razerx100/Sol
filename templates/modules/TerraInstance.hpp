#ifndef __TERRA_INSTANCE_HPP__
#define __TERRA_INSTANCE_HPP__
#include <IGraphicsEngine.hpp>

__declspec(dllimport) GraphicsEngine* __cdecl CreateTerraInstance(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	size_t bufferCount = 2u
);
#endif
