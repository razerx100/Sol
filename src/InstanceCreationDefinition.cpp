#include <App.hpp>
#include <ModelContainer.hpp>
#include <UploadBuffer.hpp>
#include <TextureAtlas.hpp>

IApp* CreateAppInstance() {
	return new App();
}

IModelContainer* CreateModelContainerInstance() {
	return new ModelContainer();
}

IUploadBuffer* CreateUploadBufferInstance() {
	return new UploadBuffer();
}

ITextureAtlas* CreateTextureAtlasInstance() {
	return new TextureAtlas();
}
