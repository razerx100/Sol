#include <App.hpp>
#include <Keyboard.hpp>
#include <Window.hpp>

App::App() {}

void App::Update() {
	if (
		Keyboard::GetRef()->IsKeyPressed(Enter)
		&&
		Keyboard::GetRef()->IsKeyPressed(Alt)
		)
		Window::GetRef()->SetTitle("Key Binding Works");
}
