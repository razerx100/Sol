#ifndef __APP_HPP__
#define __APP_HPP__
#include <IApp.hpp>

class App : public IApp {
public:
	App();

	void Update() override;
};
#endif
