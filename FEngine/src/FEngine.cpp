#include "FEngine.h"

FEngine::FEngine()
{

}

void FEngine::Run()
{

	Window window( 800,600, "Vulkan" );
	Renderer renderer(window);

	while ( window.WindowOpen() )
	{
			glfwPollEvents();
			renderer.drawFrame();
	}
}