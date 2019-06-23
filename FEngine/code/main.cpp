#include "Includes.h"

#include "vulkan/vkRenderer.h"

int main()
{
	{
		vk::Renderer renderer({ 800,600 });
		renderer.Run();
	}
	int wait;
	std::cin >> wait;

	return 0;
}