#include <Window.hpp>

void Window::Init(std::unique_ptr<Window> instance) {
	s_instance = std::move(instance);
}

Window* Window::GetRef() noexcept {
	return s_instance.get();
}
