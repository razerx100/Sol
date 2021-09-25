#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cstdint>

class __declspec(dllimport) Window {
public:
	virtual ~Window() = default;

	virtual void SetTitle(const char* title) = 0;
	virtual int Update() = 0;
	virtual void SetWindowIcon(const char* iconPath) = 0;
	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	virtual bool IsCursorEnabled() const noexcept = 0;
	virtual void* GetWindowHandle() const noexcept = 0;
};

__declspec(dllimport) Window* _cdecl GetWindowInstance() noexcept;
__declspec(dllimport) void _cdecl InitWindowInstance(
	int width, int height, const char* name
);
__declspec(dllimport) void _cdecl CleanUpWindowInstance();

#endif
