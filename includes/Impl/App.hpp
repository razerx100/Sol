#ifndef __APP_HPP__
#define __APP_HPP__
#include <IApp.hpp>
#include <BasicModels.hpp>
#include <vector>

class App : public IApp {
public:
	App();

	void SetResources() override;
	void Update() override;

private:
	std::vector<ISolModel*> m_triangleRefs;
};
#endif
