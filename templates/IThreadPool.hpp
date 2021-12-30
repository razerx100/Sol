#ifndef __I_THREAD_POOL_HPP__
#define __I_THREAD_POOL_HPP__
#include <cstdint>

class __declspec(dllimport) IThreadPool {
public:
	virtual ~IThreadPool() = default;

	virtual void SubmitWork(void* workFunction) = 0;
};
#endif
