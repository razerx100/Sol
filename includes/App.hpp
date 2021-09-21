#ifndef __APP_HPP__
#define __APP_HPP__
#include <memory>

class App {
public:
	App() = default;

	void Update();
};

App* GetAppInstance() noexcept;
void InitAppInstance() noexcept;

#endif
