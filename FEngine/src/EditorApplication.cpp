#include "EditorApplication.h"

#include "editor/EditorCamera.h"
#include "editor/GameObject.h"
#include "editor/Mesh.h"
#include "util/Time.h"

#include <map>
#include <cmath>

EditorApplication::EditorApplication()
{
	scene = new Scene("Test scene");
}

void EditorApplication::Run()
{
	// Create required components
	Window window( 1200,700, "Vulkan" );
	renderer = new Renderer(window);	
	physicsEngine = new PhysicsEngine();

	// Test rendering
	GameObject* cameraGo = scene->CreateGameobject("Editor Camera");
	EditorCamera* camera = cameraGo->AddComponent<EditorCamera>();
	cameraGo->GetComponent<Transform>()->SetPosition({ 0,0,-10 });
	camera->aspectRatio = renderer->GetAspectRatio();

	/*Triangle t{ { 0,-5,-5 },{ 0,-5,5 },{ 0,5,0 } };
	Cube c(1);
	glm::vec3 cpos = { 0,0,0 };

	glm::vec3 p2 = { 7,0,5 };
	glm::vec3 d2 = { -7.5,0,0 };
	glm::vec2 screenPos = { 400, 400 };*/

	GameObject* kiwi = scene->CreateGameobject("kiwi");
	Mesh* kiwiMesh = kiwi->AddComponent<Mesh>();
	kiwiMesh->LoadModel("mesh/kiwi.obj");
	kiwiMesh->renderId = renderer->AddMesh(kiwiMesh->vertices, kiwiMesh->indices);
	kiwi->GetComponent<Transform>()->SetPosition( { 5,0,0 });
	kiwi->GetComponent<Transform>()->SetScale({ 0.1,0.1,0.1 });

	GameObject* cube1 = scene->CreateGameobject("cube");
	Mesh* mesh1 = cube1->AddComponent<Mesh>();
	mesh1->LoadModel("mesh/cube.obj");
	mesh1->renderId = renderer->AddMesh(mesh1->vertices, mesh1->indices);

	// Intialization before the main loop
	ImGuiIO& io = ImGui::GetIO();	
	float lastTime = Time::ElapsedSinceStartup();

	std::map<GameObject*,  Cube> cubes;
	for (GameObject * gameObject : scene->GetGameObjects())
	{
		Mesh* mesh = gameObject->GetComponent<Mesh>();
		if (mesh)
		{
			float maxX = 0.f;
			float maxY = 0.f;
			float maxZ = 0.f;
			for (ForwardPipeline::Vertex vertex : mesh->vertices)
			{
				float x = std::abs(vertex.pos.x);
				if (x > maxX)
					maxX = x;
				float y = std::abs(vertex.pos.y);
				if (y > maxY)
					maxY = y;
				float z = std::abs(vertex.pos.z);
				if (z > maxZ)
					maxZ = z;
			}

			Transform* transform = gameObject->GetComponent<Transform>();
			float maxScale = std::max(transform->GetScale().x, std::max(transform->GetScale().y, transform->GetScale().z));
			float halfSize = maxScale *  std::max(maxX, std::max(maxY, maxZ));			

			cubes[gameObject] = Cube({}, halfSize);
		}			
	}
		

	// Main loop
	while ( window.WindowOpen() &&  ! m_editorShouldQuit)
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

			// camera uniforms
			renderer->UpdateCameraUniforms(camera->GetProjection(), camera->GetView());

			// Mesh Uniforms
			for (GameObject * gameObject : scene->GetGameObjects())
				for (Mesh* mesh : gameObject->GetComponents<Mesh>())
				{
					// Mesh geometry changed
					if (mesh->NeedsUpdate())
					{
						// Reload the mesh buffers in the rendere
						renderer->RemoveMesh(mesh->renderId);
						mesh->renderId = nullptr;
						mesh->renderId = renderer->AddMesh(mesh->vertices, mesh->indices);
						mesh->SetUpdated();
					}

					//Test
					for( std::pair<GameObject* const, Cube>& pair : cubes)
					{
						pair.second.SetPosition(pair.first->GetComponent<Transform>()->GetPosition());
						for (const Triangle& triangle : pair.second.GetTriangles())
						{
							renderer->DebugLine(triangle.v0, triangle.v1);							
							renderer->DebugLine(triangle.v1, triangle.v2);
							renderer->DebugLine(triangle.v2, triangle.v0);						
						}
					}

					if(mesh->renderId)
						renderer->SetModelMatrix(mesh->renderId, gameObject->GetComponent<Transform>()->GetModelMatrix());
				}

			glm::vec4 pink{ 1.f,0,1,1.f };
			glm::vec4 green{ 0,1,0,1.f };
			glm::vec4 blue{ 0,0,1,1.f };
			glm::vec4 yellow{ 1,1,0,1.f };




			// Cube pos
			/*c.SetPosition(cpos);
			renderer->DebugLine(t.v0, t.v1);
			renderer->DebugLine(t.v1, t.v2);
			renderer->DebugLine(t.v2, t.v0);*/

			//Ui
			ImGui::NewFrame();

			/*ImGui::Begin("Test");

			ImGui::DragFloat3("v0", (float*)&t.v0, 0.05f);
			ImGui::DragFloat3("v1", (float*)&t.v1, 0.05f);
			ImGui::DragFloat3("v2", (float*)&t.v2, 0.05f);
			ImGui::DragFloat3("cube position", (float*)&cpos, 0.05f);
			ImGui::DragFloat3("p2", (float*)&p2, 0.05f);
			ImGui::DragFloat3("d2", (float*)&d2, 0.05f);*/

			/*static bool test = true;
			ImGui::Checkbox("test", &test);
			ImGui::End();

			// Calculates the direction of a ray going from the mouse forward and draws it into p2, d2
			if (Mouse::KeyDown(Mouse::button2))
			{
				const glm::vec3 pos = camera->GetGameobject()->GetTransform().GetPosition();
				const glm::vec3 up = camera->GetGameobject()->GetTransform().Up();
				const glm::vec3 right = camera->GetGameobject()->GetTransform().Right();
				const glm::vec3 forward = camera->GetGameobject()->GetTransform().Forward();
				const glm::vec2 mousePos = Mouse::Position();
				const float far = camera->farp;
				const float fov = camera->fov;

				glm::vec3 nearMiddle = pos + camera->nearp * forward;

				float nearHeight = camera->nearp * tan(glm::radians(camera->fov / 2));
				float nearWidth = (screenSize.x / screenSize.y) * nearHeight;

				glm::vec2 ratio = 2.f * mousePos / screenSize - glm::vec2(1.f, 1.f);
				p2 = nearMiddle + ratio.x * nearWidth * right - ratio.y * nearHeight * up;
				d2 = 100.f * glm::normalize(p2 - camera->GetGameobject()->GetTransform().GetPosition());
			}

			renderer->DebugPoint(p2);
			renderer->DebugLine(p2, p2 + d2, yellow);*/

			//Intersections and rendering
			/*for (Triangle& tri : c.triangles)
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
				renderer->DebugPoint(intersection, blue);*/


			//renderer->DebugPoint({}, blue);

			RenderGUI();

			renderer->DrawFrame();
		}
	}

	delete(renderer);
	delete(physicsEngine);
}

void EditorApplication::RenderGUI()
{
	// Main Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		// File
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Button("Quit"))
				m_editorShouldQuit = true;
			ImGui::EndMenu();
		}

		// Window
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::Checkbox("Renderer", &m_showRendererWindow);			
			ImGui::Checkbox("ShowSceneHierarchy", &m_showSceneHierarchy);
			ImGui::Checkbox("ShowInspector", &m_showInspector);
			ImGui::Checkbox("TestWindow", &m_showTestWindow);
			ImGui::EndMenu();
		}

		// Help
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Text("Controle camera: ZSQDAE avec clic droit appuye");
			ImGui::Text("Clic molette pour caster des rayons");
			ImGui::Text("Code par Francois Candela - 2018");
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}	

	// Renderer window
	if (m_showRendererWindow)
		renderer->RenderGUI();
	
	// Tests window
	if(m_showTestWindow)
		ImGui::ShowTestWindow();

	// Scene Hierarchy
	if (m_showSceneHierarchy)
		scene->RenderSceneGui();
	
	// nspector
	if (m_showInspector)
		scene->RenderInspectorGui();
}