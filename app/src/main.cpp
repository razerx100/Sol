#include <Engine.hpp>
#include <Exception.hpp>
#include <ExceptionMessageBox.hpp>

int main()
{
	try
	{
		return Engine().Run();
	}
	catch (const Exception& e)
	{
		ExceptionMessageBox(e.what(), e.GetType());
	}
	catch (const std::exception& e)
	{
		ExceptionMessageBox(e.what(), "Standard Exception");
	}
	catch (...)
	{
		ExceptionMessageBox("No details available", "Unknown Exception");
	}

	return -1;
}
