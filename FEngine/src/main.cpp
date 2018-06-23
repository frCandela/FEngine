#include "FEngine.h"

int main()
{
	FEngine app;

	try 
	{
		app.Run();
	}
	catch (const std::runtime_error& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	int zob;
	std::cin >> zob;

	return EXIT_SUCCESS;
}