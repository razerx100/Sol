#include <App.hpp>
#include <Window.hpp>
#include <InputManager.hpp>
#include <string>

void App::Update() {
	std::string pointer = std::to_string(
		GetInputManagerInstance()->GetMouseByIndex()->GetPosX()
	) + " " + std::to_string(
		GetInputManagerInstance()->GetMouseByIndex()->GetPosY()
	);

	GetWindowInstance()->SetTitle(pointer.c_str());
}
