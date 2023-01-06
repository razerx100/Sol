#ifndef APP_HPP_
#define APP_HPP_
#include <vector>
#include <memory>
#include <string>

class Model;

class App {
public:
	App();

	void SetResources();
	void PerFrameUpdate();
	void PhysicsUpdate();
};
#endif
