#include "fanGlobalIncludes.h"

#include "fanEngine.h"

int main()
{	
	{
		fan::Engine engine;
		engine.Run();
	}
	
	int wait;
	std::cin >> wait;

	return 0;
}