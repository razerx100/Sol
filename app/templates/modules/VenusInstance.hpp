#ifndef VENUS_INSTANCE_HPP_
#define VENUS_INSTANCE_HPP_
#include <IThreadPool.hpp>

__declspec(dllimport) IThreadPool* __cdecl CreateVenusInstance(size_t threadCount);
#endif
