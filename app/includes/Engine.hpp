#ifndef ENGINE_HPP_
#define ENGINE_HPP_
#include <string>
#include <ObjectManager.hpp>

class Engine {
public:
	Engine();

	[[nodiscard]]
	int Run();

private:
	const std::string m_appName;
	ObjectManager m_objectManager;
};
#endif
