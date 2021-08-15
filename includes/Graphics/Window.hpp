#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>

class Window {
public:
	virtual ~Window() = default;

	virtual void Update() = 0;
	virtual void* GetWindowHandle() = 0;
	virtual void SetWindowSize(std::uint32_t width, std::uint32_t height) = 0;
};
#endif
