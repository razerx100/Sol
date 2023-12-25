#ifndef TERRA_INSTANCE_HPP_
#define TERRA_INSTANCE_HPP_
#include <Renderer.hpp>
#include <IThreadPool.hpp>
#include <ISharedDataContainer.hpp>

#ifdef BUILD_TERRA
#define TERRA_DLL __declspec(dllexport)
#else
#define TERRA_DLL __declspec(dllimport)
#endif

TERRA_DLL Renderer* __cdecl CreateTerraInstance(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	IThreadPool& threadPool, ISharedDataContainer& sharedContainer,
	RenderEngineType engineType,
	std::uint32_t bufferCount = 2u
);
#endif
