#ifndef APP_MODULES_HPP_
#define APP_MODULES_HPP_
#include <memory>
#include <TimeManager.hpp>

#include <CameraManagerSol.hpp>

namespace AMods {
	extern std::unique_ptr<FrameTime> frameTime;
	extern std::unique_ptr<CameraManagerSol> cameraManager;

	void InitFrameTime();
	void InitCameraManager();

	void InitAppModules();
	void ResetAppModules();
}
#endif
