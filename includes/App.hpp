#ifndef __APP_HPP__
#define __APP_HPP__
#include <memory>
#include <GenericSingleton.hpp>

class App : public GenericSingleton<App> {
public:
	App();

	void Update();
};
#endif
