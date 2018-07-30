#include "FEngine.h"

#include "renderer/Camera.h"
#include "editor/GameObject.h"
#include "util/Time.h"

FEngine::FEngine()
{

}

void FEngine::Run()
{
	Window window( 1200,700, "Vulkan" );

	GameObject gameobject;
	Camera* camera = gameobject.AddComponent<Camera>();
	renderer = new Renderer(window, *camera);	

	float lastTime = Time::ElapsedSinceStartup();

	ImGuiIO& io = ImGui::GetIO();

	while ( window.WindowOpen() )
	{
		float time = Time::ElapsedSinceStartup();

		if (Time::ElapsedSinceStartup() - lastTime > renderer->framerate.GetDelta())
		{
			// Updates Imgui io and starts new imgui frame
			io.DeltaTime = time - lastTime;
			lastTime = time;
			Input::Update();
			glm::vec2 size = renderer->GetSize();
			camera->aspectRatio = size.x / size.y;
			io.DisplaySize = ImVec2(size.x, size.y);		

			renderer->DebugLine({ 0,0,0 }, { 0,0,2 });

			ImGui::NewFrame();	
			RenderGUI();
			
			renderer->DrawFrame();
		}
	}
	delete(renderer);
}

void FEngine::RenderGUI()
{
	static bool showRenderer = true;
	static bool showTestWindow = false;

	// Main Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::Checkbox("Renderer", &showRenderer);
			ImGui::Checkbox("TestWindow", &showTestWindow);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}	

	// Renderer window
	if (showRenderer)
		renderer->RenderGUI();
	
	// Tests window
	if(showTestWindow)
		ImGui::ShowTestWindow();
}