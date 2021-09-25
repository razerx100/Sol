#ifndef __APP_HPP__
#define __APP_HPP__
#include <cstdint>

class App {
public:
	App() = default;

	void Update();
};

App* GetAppInstance() noexcept;
void InitAppInstance();
void CleanUpAppInstance();

#endif
