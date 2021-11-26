#ifndef __APP_HPP__
#define __APP_HPP__
#include <IApp.hpp>
#include <BasicModels.hpp>

class App : public IApp {
public:
	App();

	void Update() override;

private:
	Triangle m_triangle;
};
#endif
