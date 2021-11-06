#include <App.hpp>
#include <Window.hpp>
#include <InputManager.hpp>
#include <string>
#include <GraphicsEngine.hpp>

App::App() {}

void App::Update() {
	IKeyboard* pKeyboardRef = GetInputManagerInstance()->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		GetWindowInstance()->SetTitle("Alt + D");

	IMouse* pMouseRef = GetInputManagerInstance()->GetMouseByIndex();

	if (pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Middle))
		GetWindowInstance()->SetTitle("Left + Right");

	IGamepad* pGamepadRef = GetInputManagerInstance()->GetGamepadByIndex();

	if (auto e = pGamepadRef->Read(); e.GetType() == IGamepad::Event::Type::LeftThumbStick)
		GetWindowInstance()->SetTitle((
			"M: " + std::to_string(e.GetMagnitude()) +
			" X: " + std::to_string(e.GetASData().xDirection) +
			" Y: " + std::to_string(e.GetASData().yDirection)
			).c_str());

	if (pGamepadRef->AreButtonsPressed(2, XBoxButton::START, XBoxButton::X))
		GetWindowInstance()->SetTitle("Start + A");
}
