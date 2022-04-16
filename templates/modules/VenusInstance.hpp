#ifndef VENUS_INSTANCE_HPP_
#define VENUS_INSTANCE_HPP_
#include <IThreadPool.hpp>

__declspec(dllimport) IThreadPool* __cdecl GetVenusInstance() noexcept;
__declspec(dllimport) void __cdecl CreateVenusInstance(size_t threadCount);
__declspec(dllimport) void __cdecl CleanUpVenusInstance() noexcept;
#endif
