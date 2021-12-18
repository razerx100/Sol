#include <App.hpp>
#include <GraphicsEngine.hpp>
#include <InstanceManager.hpp>
#include <CRSColor.hpp>
#include <string>
#include <BasicModels.hpp>

App::App() {
	RendererInst::GetRef()->SubmitModels(new Triangle(Ceres::Color::Cyan), 1u, false);
	RendererInst::GetRef()->SubmitModels(new Triangle1(Ceres::Color::Fuchsia), 1u, false);
}

void App::Update() {
	IKeyboard* pKeyboardRef = IOInst::GetRef()->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		WindowInst::GetRef()->SetTitle("Alt + D");

	IMouse* pMouseRef = IOInst::GetRef()->GetMouseByIndex();

	if (pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Middle))
		WindowInst::GetRef()->SetTitle("Left + Right");

	IGamepad* pGamepadRef = IOInst::GetRef()->GetGamepadByIndex();

	if (auto e = pGamepadRef->Read(); e.GetType() == IGamepad::Event::Type::LeftThumbStick)
		WindowInst::GetRef()->SetTitle((
			"M: " + std::to_string(e.GetMagnitude()) +
			" X: " + std::to_string(e.GetASData().xDirection) +
			" Y: " + std::to_string(e.GetASData().yDirection)
			).c_str());

	if (pGamepadRef->AreButtonsPressed(2, XBoxButton::START, XBoxButton::X))
		WindowInst::GetRef()->SetTitle("Start + A");
}
