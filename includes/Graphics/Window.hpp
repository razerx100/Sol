#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>
#include <optional>
#include <string>

class Window {
public:
	virtual ~Window() = default;

	inline static void Init(std::unique_ptr<Window> instance) {
		s_instance = std::move(instance);
	}

	inline static Window* GetRef() noexcept {
		return s_instance.get();
	}

	virtual void SetTitle(const std::string& title) = 0;
	virtual std::optional<int> Update() = 0;
	virtual void* GetWindowHandle() const noexcept = 0;

private:
	static std::unique_ptr<Window> s_instance;
};
#endif
