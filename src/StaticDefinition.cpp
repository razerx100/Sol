#include <App.hpp>

static App* s_pApp = nullptr;

App* GetAppInstance() noexcept {
	return s_pApp;
}

void InitAppInstance() {
	if (!s_pApp)
		s_pApp = new App();
}

void CleanUpAppInstance() {
	if (s_pApp)
		delete s_pApp;
}
