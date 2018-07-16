#include "FEngine.h"

#include "Camera.h"
#include "GameObject.h"

FEngine::FEngine()
{

}

void FEngine::Run()
{

	Window window( 800,600, "Vulkan" );

	GameObject gameobject;
	Camera* camera = gameobject.AddComponent<Camera>();

	Renderer renderer(window, *camera);

	camera->aspectRatio = renderer.swapChain->swapChainExtent.width / (float) renderer.swapChain->swapChainExtent.height;
	
	//camera->gameobject = &gameobject;


	while ( window.WindowOpen() )
	{
			glfwPollEvents();
			renderer.drawFrame();
	}
}