#include "FEngine.h"

#include "Camera.h"
#include "GameObject.h"

FEngine::FEngine()
{

}

void FEngine::Run()
{

	Window window( 1500,900, "Vulkan" );

	GameObject gameobject;
	Camera* camera = gameobject.AddComponent<Camera>();
	Renderer renderer(window, *camera);
	//camera->aspectRatio = size.x / size.y;
	while ( window.WindowOpen() )
	{
		Input::Update();
		



		ImGui::ShowTestWindow();
		
		renderer.drawFrame();

	}
}