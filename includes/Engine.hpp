#ifndef ENGINE_HPP_
#define ENGINE_HPP_
#include <string>

class Engine {
public:
	Engine();
	~Engine() noexcept;

	[[nodiscard]]
	int Run();

	void WaitForAsyncTasks();

private:
	const std::string m_appName;
};
#endif
