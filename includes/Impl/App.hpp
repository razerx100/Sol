#ifndef __APP_HPP__
#define __APP_HPP__
#include <IApp.hpp>
#include <BasicModels.hpp>
#include <memory>
#include <vector>

class App : public IApp {
public:
	App();

	void Update() override;

private:
	std::vector<std::unique_ptr<ISolModel>> m_triangles;
};
#endif
