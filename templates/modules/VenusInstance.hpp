#ifndef __VENUS_INSTANCE_HPP__
#define __VENUS_INSTANCE_HPP__
#include <IThreadPool.hpp>
#include <cstdint>

__declspec(dllimport) IThreadPool* __cdecl GetVenusInstance() noexcept;
__declspec(dllimport) void __cdecl CreateVenusInstance(size_t threadCount);
__declspec(dllimport) void __cdecl CleanUpVenusInstance() noexcept;
#endif
