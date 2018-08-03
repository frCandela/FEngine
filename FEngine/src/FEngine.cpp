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
	FPSCamera* camera = gameobject.AddComponent<FPSCamera>();
	camera->position = {0,0,-10};

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
		float deltaTime = time - lastTime;
		if (deltaTime > renderer->framerate.GetDelta())
		{
			// Updates Imgui io and starts new imgui frame
			io.DeltaTime = time - lastTime;
			lastTime = time;
			Input::Update();
			glm::vec2 screenSize = renderer->GetSize();
			camera->aspectRatio = screenSize.x / screenSize.y;
			io.DisplaySize = ImVec2(screenSize.x, screenSize.y);
			camera->Update(deltaTime);

			renderer->UpdateCameraUniforms(camera->GetProjection(), camera->GetView());

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
			ImGui::DragFloat3("p2", (float*)&p2, 0.05f);
			ImGui::DragFloat3("d2", (float*)&d2, 0.05f);

			static bool test = true;
			ImGui::Checkbox("test", &test);
			ImGui::End();

			// Calculates the direction of a ray going from the mouse forward and draws it into p2, d2
			if (Mouse::KeyDown(Mouse::button3))
			{
				const glm::vec3 pos = camera->position;
				const glm::vec3 up = camera->Up();
				const glm::vec3 right = camera->Right();
				const glm::vec3 forward = camera->Forward();
				const glm::vec2 mousePos = Mouse::Position();
				const float far = camera->farp;
				const float fov = camera->fov;

				glm::vec3 nearMiddle = pos + camera->nearp * forward;

				float nearHeight = camera->nearp * tan(glm::radians(camera->fov / 2));
				float nearWidth = (screenSize.x / screenSize.y) * nearHeight;

				glm::vec2 ratio = 2.f * mousePos / screenSize - glm::vec2(1.f, 1.f);
				p2 = nearMiddle + ratio.x * nearWidth * right - ratio.y * nearHeight * up;
				d2 = 100.f * glm::normalize(p2 - camera->position);
			}

			renderer->DebugPoint(p2);
			renderer->DebugLine(p2, p2 + d2, yellow);


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