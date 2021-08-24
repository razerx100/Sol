#include <App.hpp>
#include <Keyboard.hpp>
#include <Window.hpp>

App::App() {}

void App::Update() {
	if (Keyboard::GetRef()->IsKeyPressed(Enter) && Keyboard::GetRef()->IsKeyPressed(Alt))
		Window::GetRef()->SetTitle("Key Binding Works");
}

App* App::GetRef() noexcept {
	return s_instance.get();
}

void App::Init() {
	s_instance = std::make_unique<App>();
}
