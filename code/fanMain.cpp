#include "fanGlobalIncludes.h"

#include "fanEngine.h"

void RunGame();

int main()
{	
	RunGame();

	while(true);

	return 0;
}

// Can you guess what this function does :3
void RunGame() {

	fan::Engine engine;
	engine.Run();
}
