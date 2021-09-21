#include <App.hpp>
#include <IKeyboard.hpp>
#include <Window.hpp>

void App::Update() {
	if (
		GetKeyboardInstance()->IsKeyPressed(Enter)
		&&
		GetKeyboardInstance()->IsKeyPressed(Alt)
		)
		GetWindowInstance()->SetTitle("Key Binding Works");
}
