#include <App.hpp>
#include <Window.hpp>
#include <InputManager.hpp>
#include <string>

void App::Update() {
	IKeyboard* pKeyboardRef = GetInputManagerInstance()->GetKeyboardByIndex();

	if(pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		GetWindowInstance()->SetTitle("Alt + D");

	IMouse* pMouseRef = GetInputManagerInstance()->GetMouseByIndex();

	if(pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Left))
		GetWindowInstance()->SetTitle("Left + Right");
}
