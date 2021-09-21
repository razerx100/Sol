#include <App.hpp>

static std::unique_ptr<App> sApp;

App* GetAppInstance() noexcept {
	return sApp.get();
}

void InitAppInstance() noexcept {
	sApp = std::make_unique<App>();
}
