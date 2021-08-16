#include <Engine.hpp>
#include <Exception.hpp>

int main() {
	try {
		return Engine().Run();
	}
	catch (const Exception& e) {}
	catch (const std::exception& e) {}
	catch (...) {}
	return -1;
}
