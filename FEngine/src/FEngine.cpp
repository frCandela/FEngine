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

	glm::vec3 p2 = { 7,0,5 };
	glm::vec3 d2 = { -7.5,0,0 };
	glm::vec2 screenPos = { 400, 400 };

	while ( window.WindowOpen() )
	{
		float time = Time::ElapsedSinceStartup();

		if (Time::ElapsedSinceStartup() - lastTime > renderer->framerate.GetDelta())
		{
			// Updates Imgui io and starts new imgui frame
			io.DeltaTime = time - lastTime;
			lastTime = time;
			Input::Update();
			glm::vec2 screenSize = renderer->GetSize();
			camera->aspectRatio = screenSize.x / screenSize.y;
			io.DisplaySize = ImVec2(screenSize.x, screenSize.y);

			glm::vec4 pink{ 1.f,0,1,1.f };
			glm::vec4 green{ 0,1,0,1.f };
			glm::vec4 blue{ 0,0,1,1.f };
			glm::vec4 yellow{ 1,1,0,1.f };

			// Cube pos
			c.SetPosition(cpos);
			renderer->DebugLine(t.v0, t.v1);
			renderer->DebugLine(t.v1, t.v2);
			renderer->DebugLine(t.v2, t.v0);

			//Ui
			ImGui::NewFrame();
			ImGui::Begin("Test");
			ImGui::DragFloat3("v0", (float*)&t.v0, 0.05f);
			ImGui::DragFloat3("v1", (float*)&t.v1, 0.05f);
			ImGui::DragFloat3("v2", (float*)&t.v2, 0.05f);

			ImGui::DragFloat3("cube position", (float*)&cpos, 0.05f);

			static bool follow = true;
			ImGui::Checkbox("follow mouse", &follow);

				float far = camera->farp;
				float fov = camera->fov;
				float nearHeight = camera->nearp * tan(glm::radians(camera->fov / 2));
				float nearWidth = (screenSize.x / screenSize.y) * nearHeight;
				  
				glm::vec3 nearMiddle = camera->pos + camera->nearp * camera->dir;
				glm::vec3 up =  { 0,1,0 };
				glm::vec3 right = glm::cross(camera->dir, up);
				glm::vec3 forward = camera->dir;
				glm::vec2 mousePos = Mouse::Position();
				glm::vec2 ratio = 2.f * mousePos / screenSize - glm::vec2(1.f,1.f);
				p2 = nearMiddle + ratio.x * nearWidth * right - ratio.y * nearHeight * up;
				d2 = 100.f *  glm::normalize( p2 - camera->pos);

				//Intersections and rendering
				for (Triangle& tri : c.triangles)
				{
					renderer->DebugLine(tri.v0, tri.v1);
					renderer->DebugLine(tri.v1, tri.v2);
					renderer->DebugLine(tri.v2, tri.v0);

					glm::vec3 intersection;
					if (tri.RayCast(p2, d2, &intersection))
						renderer->DebugPoint(intersection, green);
				}
				glm::vec3 intersection;
				if (t.RayCast(p2, d2, &intersection))
					renderer->DebugPoint(intersection, blue);
				
				renderer->DebugPoint(p2 + d2, green);
			
			ImGui::DragFloat3("p2", (float*)&p2, 0.05f);
			ImGui::DragFloat3("d2", (float*)&d2, 0.05f);			






			renderer->DebugLine(p2, p2 + d2, yellow);

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