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

	glm::vec3 v0 = { 0,-5,-5 };
	glm::vec3 v1 = { 0,-5,5 };
	glm::vec3 v2 = { 0,5,0};
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

			using namespace glm;

			bool intersect = true;
			vec3 intersectionPoint;

			const float EPSILON = 0.0000001;
			vec3 e1 = v1 - v0;	// edge 1
			vec3 e2 = v2 - v0;	// edge 2
			vec3 h = cross(d, e2);	// 
			float a = dot(e1, h);  
			if (a > -EPSILON && a < EPSILON)	// d colinear to the e1 e2 plane
				intersect = false;
			else
			{
				float f = 1 / a;
				vec3 s = p - v0;
				float u = f * dot(s,h);
				if (u < 0.0 || u > 1.0)
					intersect =  false;
				else
				{
					vec3 q = cross(s,e1);
					float v = f * dot(d,q);
					if (v < 0.0 || u + v > 1.0)
						intersect = false;
					else
					{
						// At this stage we can compute t to find out where the intersection point is on the line.
						float t = f * dot(e2,q);
						if (t > EPSILON) // ray intersection
						{
							intersectionPoint = p + d * t;
							intersect = true;
						}
						else // This means that there is a line intersection but not a ray intersection.
							intersect = false;
					}
				}
			}

			renderer->DebugLine(v0, v1);
			renderer->DebugLine(v1, v2);
			renderer->DebugLine(v2, v0);
			renderer->DebugLine(p, p + d, pink);
			//renderer->DebugLine(p1, p1+ 10.f*n, green);
			if(intersect)
				renderer->DebugPoint(intersectionPoint, blue);

			//TEST
			ImGui::NewFrame();
			ImGui::Begin("Test");
			ImGui::DragFloat3("v0", (float*)&v0, 0.05f);
			ImGui::DragFloat3("v1", (float*)&v1, 0.05f);
			ImGui::DragFloat3("v2", (float*)&v2, 0.05f);
			ImGui::DragFloat3("p", (float*)&p, 0.05f);
			ImGui::DragFloat3("d", (float*)&d, 0.05f);
			ImGui::NewLine();
			ImGui::Checkbox("intersect", &intersect);
			//ImGui::DragFloat("d", (float*)&d, 0.05f);
			
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