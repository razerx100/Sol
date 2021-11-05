#ifndef __APP_HPP__
#define __APP_HPP__
#include <cstdint>

class App {
public:
	App();

	void Update();
};

App* GetAppInstance() noexcept;
void InitAppInstance();
void CleanUpAppInstance() noexcept;
#endif
