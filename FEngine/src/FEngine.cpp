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

	while ( window.WindowOpen() )
	{
		Input::Update();

		// Update imGui
		/*ImGuiIO& io = ImGui::GetIO();
		glm::vec2 size = renderer.GetSize();
		io.DisplaySize = ImVec2(size.x, size.y);
		io.DeltaTime = 1 / 60.f;
		io.MousePos = ImVec2(Mouse::Position().x, Mouse::Position().y);
		io.MouseDown[0] = Mouse::KeyDown(Mouse::left);
		io.MouseDown[1] = Mouse::KeyDown(Mouse::right);*/

		glfwPollEvents();
		renderer.drawFrame();
	}
}