#ifndef I_THREAD_POOL_HPP_
#define I_THREAD_POOL_HPP_
#include <functional>

class __declspec(dllimport) IThreadPool {
public:
	virtual ~IThreadPool() = default;

	virtual void SubmitWork(std::function<void()> workFunction) = 0;
};
#endif
