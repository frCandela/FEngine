#include "AllInclude.h"

#include "vkRenderer.h"

int main()
{
	{
		Renderer renderer({ 800,600 });
		renderer.Run();
	}
	int wait;
	std::cin >> wait;

	return 0;
}