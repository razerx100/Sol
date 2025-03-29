#ifndef APP_HPP_
#define APP_HPP_
#include <Renderer.hpp>
#include <InputManager.hpp>
#include <ExtensionManager.hpp>
#include <RenderPassManager.hpp>

class App
{
public:
	App(
		Renderer* renderer, InputManager* inputManager, RenderEngineType engineType,
		ExtensionManager* extensionManager, RenderPassManager* renderPassManager,
		std::uint32_t frameCount
	);

	void Init();

	void PhysicsUpdate();

private:
	Renderer*                      m_renderer;
	BlinnPhongLightTechnique*      m_blinnPhong;
	WeightedTransparencyTechnique* m_transparencyPass;
	InputManager*                  m_inputManager;
	RenderEngineType               m_engineType;
	RenderPassManager*             m_renderPassManager;
};
#endif
