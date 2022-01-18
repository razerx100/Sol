#include <App.hpp>
#include <ModelContainer.hpp>

IApp* CreateAppInstance() {
	return new App();
}

IModelContainer* CreateModelContainerInstance() {
	return new ModelContainer();
}
