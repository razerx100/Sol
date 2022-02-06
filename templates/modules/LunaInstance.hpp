#ifndef __LUNA_INSTANCE_HPP__
#define __LUNA_INSTANCE_HPP__
#include <Window.hpp>

[[nodiscard]]
__declspec(dllimport) Window* __cdecl CreateLunaInstance(
	std::uint32_t width, std::uint32_t height, class InputManager* ioMan, const char* name
);
#endif
