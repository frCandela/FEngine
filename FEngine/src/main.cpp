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
	}

	int wait;
	std::cin >> wait;
	return EXIT_SUCCESS;
}