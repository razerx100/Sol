#ifndef APP_HPP_
#define APP_HPP_
#include <BasicModels.hpp>
#include <vector>

class App {
public:
	App();

	void SetResources();
	void Update();

private:
	std::vector<Model*> m_triangleRefs;
};
#endif
