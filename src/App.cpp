#include <App.hpp>
#include <Keyboard.hpp>
#include <Window.hpp>

std::unique_ptr<App> App::s_instance;

App::App() {

}

void App::Update() {
	if (Keyboard::GetRef()->IsKeyPressed(SpaceBar))
		Window::GetRef()->SetTitle("Key Binding Works");
}

App* App::GetRef() noexcept {
	return s_instance.get();
}

void App::Init() {
	s_instance = std::make_unique<App>();
}
