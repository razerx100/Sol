#ifndef LUNA_INSTANCE_HPP_
#define LUNA_INSTANCE_HPP_
#include <Window.hpp>

[[nodiscard]]
__declspec(dllimport) Window* __cdecl CreateLunaInstance(
	std::uint32_t width, std::uint32_t height, const char* name
);
#endif
