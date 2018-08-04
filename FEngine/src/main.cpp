#include "EditorApplication.h"

int main()
{
	EditorApplication app;

	try 
	{
		app.Run();
	}
	catch (const std::runtime_error& e) 
	{
		std::cerr << e.what() << std::endl;
	}

	int wait;
	std::cin >> wait;
	return EXIT_SUCCESS;
}