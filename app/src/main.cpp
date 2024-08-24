#include <Sol.hpp>
#include <Exception.hpp>
#include <ExceptionMessageBox.hpp>

int main()
{
	try
	{
		Sol sol{ "Sol" };

		return sol.Run();
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
