#include <AppModules.hpp>

namespace AMods {
	std::unique_ptr<FrameTime> frameTime;
	std::unique_ptr<CameraManagerSol> cameraManager;

	void InitFrameTime() {
		frameTime = std::make_unique<FrameTime>();
	}

	void InitCameraManager() {
		cameraManager = std::make_unique<CameraManagerSol>();
	}

	void InitAppModules() {
		InitFrameTime();
		InitCameraManager();
	}

	void ResetAppModules() {
		frameTime.reset();
		cameraManager.reset();
	}
}
