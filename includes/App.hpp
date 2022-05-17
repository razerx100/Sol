#ifndef APP_HPP_
#define APP_HPP_
#include <vector>

class App {
public:
	App();

	void SetResources();
	void Update();

private:
	std::vector<class Model*> m_modelRefs;
};
#endif
