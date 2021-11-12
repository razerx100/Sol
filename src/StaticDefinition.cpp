#include <App.hpp>

static IApp* s_pApp = nullptr;

IApp* GetAppInstance() noexcept {
	return s_pApp;
}

void InitAppInstance() {
	if (!s_pApp)
		s_pApp = new App();
}

void CleanUpAppInstance() noexcept {
	if (s_pApp)
		delete s_pApp;
}
