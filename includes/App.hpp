#ifndef __APP_HPP__
#define __APP_HPP__
#include <memory>

class App {
public:
	App();

	static void Init();
	static App* GetRef() noexcept;

	void Update();

private:
	static std::unique_ptr<App> s_instance;
};
#endif
