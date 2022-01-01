#ifndef __I_APP_HPP__
#define __I_APP_HPP__
#include <cstdint>

class IApp {
public:
	virtual ~IApp() = default;

	virtual void Update() = 0;
};

[[nodiscard]]
IApp* CreateAppInstance();
#endif
