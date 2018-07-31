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
	physicsEngine = new PhysicsEngine();

	float lastTime = Time::ElapsedSinceStartup();

	ImGuiIO& io = ImGui::GetIO();

	Triangle t{ { 0,-5,-5 },{ 0,-5,5 },{ 0,5,0 } };
	Cube c(3);
	glm::vec3 cpos = { 0,0,0 };

	glm::vec3 p = { 7,0,0 };
	glm::vec3 d = { -7.5,0,0 };


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

			glm::vec4 pink{ 1.f,0,1,1.f };
			glm::vec4 green{ 0,1,0,1.f };
			glm::vec4 blue{ 0,0,1,1.f };

			renderer->DebugLine(t.v0, t.v1);
			renderer->DebugLine(t.v1, t.v2);
			renderer->DebugLine(t.v2, t.v0);
			renderer->DebugLine(p, p + d, pink);

			c.SetPosition(cpos);

			for (Triangle& tri : c.triangles)
			{
				renderer->DebugLine(tri.v0, tri.v1);
				renderer->DebugLine(tri.v1, tri.v2);
				renderer->DebugLine(tri.v2, tri.v0);

				glm::vec3 intersection;
				if (tri.RayCast(p, d, &intersection))
					renderer->DebugPoint(intersection, green);
			}

			glm::vec3 intersection;
			if( t.RayCast( p, d, &intersection) )
				renderer->DebugPoint(intersection, blue);

			//TEST
			ImGui::NewFrame();
			ImGui::Begin("Test");
			ImGui::DragFloat3("v0", (float*)&t.v0, 0.05f);
			ImGui::DragFloat3("v1", (float*)&t.v1, 0.05f);
			ImGui::DragFloat3("v2", (float*)&t.v2, 0.05f);
			ImGui::DragFloat3("p", (float*)&p, 0.05f);
			ImGui::DragFloat3("d", (float*)&d, 0.05f);
			ImGui::DragFloat3("cube position", (float*)&cpos, 0.05f);
			
			ImGui::End();



			RenderGUI();
			
			renderer->DrawFrame();
		}
	}

	delete(renderer);
	delete(physicsEngine);
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