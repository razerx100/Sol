#ifndef APP_HPP_
#define APP_HPP_
#include <Renderer.hpp>
#include <InputManager.hpp>

class App
{
public:
	App(Renderer* renderer, InputManager* inputManager, RenderEngineType engineType)
		: m_renderer{ renderer }, m_inputManager{ inputManager }, m_engineType{ engineType }
	{}

	void Init();

	void PhysicsUpdate();

private:
	Renderer*        m_renderer;
	InputManager*    m_inputManager;
	RenderEngineType m_engineType;
};
#endif
