#include <Engine.hpp>
#include <exception>

int main() {
	try {
		return Engine().Run();
	}
	catch (std::exception& e) {}
	return -1;
}
