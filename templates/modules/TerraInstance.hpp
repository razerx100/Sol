#ifndef TERRA_INSTANCE_HPP_
#define TERRA_INSTANCE_HPP_
#include <Renderer.hpp>

__declspec(dllimport) Renderer* __cdecl CreateTerraInstance(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	std::uint32_t bufferCount = 2u
);
#endif
