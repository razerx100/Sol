#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>
#include <optional>
#include <string>

class Window {
public:
	virtual ~Window() = default;

	virtual void SetTitle(const std::string& title) = 0;
	virtual std::optional<int> Update() = 0;
	virtual void* GetWindowHandle() = 0;
	virtual void SetWindowSize(std::uint32_t width, std::uint32_t height) = 0;
};
#endif
