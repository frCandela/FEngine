#include "EditorApplication.h"

#include "editor/GameObject.h"
#include "editor/Mesh.h"
#include "util/Time.h"

#include <cmath>
#include "glm/gtx/norm.hpp"

void EditorApplication::UpdateGameobjectAABB(GameObject * gameobject, bool updateGeometry)
{
	Mesh* mesh = gameobject->GetComponent<Mesh>();
	if (mesh)
	{
		if (updateGeometry)
		{
			// Recreate the AABB using to the mesh geometry

			if (mesh)
			{
				float maxX = 0.f;
				float maxY = 0.f;
				float maxZ = 0.f;

				// Iterates throught every vertex to find the boundaries
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

				Transform* transform = gameobject->GetComponent<Transform>();
				float maxScale = std::max(std::abs(transform->GetScale().x), std::max(std::abs(transform->GetScale().y), std::abs(transform->GetScale().z)));
				float halfSize = std::max(maxX, std::max(maxY, maxZ));

				m_gameObjectsAABB[gameobject] = std::make_pair(Cube(transform->GetPosition(), maxScale * halfSize), halfSize);
			}
		}
		else
		{
			// Update the AABB position and scale
			Transform* transform = gameobject->GetComponent<Transform>();
			float maxScale = std::max(std::abs(transform->GetScale().x), std::max(std::abs(transform->GetScale().y), std::abs(transform->GetScale().z)));
			float halfSize = m_gameObjectsAABB[gameobject].second;

			m_gameObjectsAABB[gameobject].first = Cube(transform->GetPosition(), maxScale * halfSize);
		}
	}		
}

void EditorApplication::ProcessComponentsModifications()
{
	// Iterates over all gameobjects and components
	for (GameObject * gameObject : m_scene->GetGameObjects())
	{
		bool updateAABBGeometry = false;		
		bool hasAABB = false;

		// Transform modified
		Transform* transform = gameObject->GetComponent<Transform>();
		bool transformModified = transform->WasModified();

		// Mesh modified
		for (Mesh* mesh : gameObject->GetComponents<Mesh>())
		{
			hasAABB = true;

			// Mesh geometry changed
			if (mesh->WasModified())
			{
				// Reload the mesh buffers in the rendere
				m_renderer->RemoveMesh(mesh->renderId);
				mesh->renderId = nullptr;
				mesh->renderId = m_renderer->AddMesh(mesh->vertices, mesh->indices);
				updateAABBGeometry = true;
			}

			// Update model matrix
			if (mesh->renderId && transformModified)
				m_renderer->SetModelMatrix(mesh->renderId, gameObject->GetComponent<Transform>()->GetModelMatrix());
		}

		// Update gameobject AABB
		if (hasAABB && (updateAABBGeometry || transformModified))
			UpdateGameobjectAABB(gameObject, updateAABBGeometry);
	}
}

void EditorApplication::SelectGameobject()
{
	// mousepos -> screenspace position
	glm::vec2 ratio = 2.f * Mouse::Position() / m_renderer->GetSize() - glm::vec2(1.f, 1.f);
	ratio.x = std::clamp(ratio.x, -1.f, 1.f);
	ratio.y = std::clamp(ratio.y, -1.f, 1.f);

	Ray ray = m_editorCamera->ScreenPosToRay(ratio);

	GameObject * closestTarget = nullptr;
	float closestDistance = std::numeric_limits<float>::max();

	// Iterates through every gameobjects AABB to find the closest one
	for (std::pair<GameObject* const, std::pair<Cube, float > >& pair : m_gameObjectsAABB)
	{
		for (const Triangle& triangle : pair.second.first.GetTriangles())
		{
			glm::vec3 intersection;
			if (triangle.RayCast(ray.origin, ray.direction, &intersection))
			{
				float distance = glm::distance2(intersection, ray.origin);
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestTarget = pair.first;
				}
			}
		}
	}

	// Set scene selected gameobject
	m_scene->SetSelectedGameobject(closestTarget);
}

void EditorApplication::DrawSelectionAABB()
{
	// Shows the AABB of the selected gameobject
	if (m_scene->GetSelectedGameobject())
	{
		auto it = m_gameObjectsAABB.find(m_scene->GetSelectedGameobject());

		// If the AABB exists
		if (it != m_gameObjectsAABB.end())
		{
			Cube& cube = it->second.first;
			for (const Triangle& triangle : cube.GetTriangles())
			{
				m_renderer->DebugLine(triangle.v0, triangle.v1);
				m_renderer->DebugLine(triangle.v1, triangle.v2);
				m_renderer->DebugLine(triangle.v2, triangle.v0);
			}
		}
	}
}

void EditorApplication::Run()
{
	// Create required modules
	Window window( 1200,700, "Vulkan" );
	m_renderer = new Renderer(window);
	m_physicsEngine = new PhysicsEngine();
	m_scene = new Scene("Test scene");

	// Create editor camera
	GameObject* cameraGo = m_scene->CreateGameobject("Editor Camera");
	m_editorCamera = cameraGo->AddComponent<EditorCamera>();
	cameraGo->GetComponent<Transform>()->SetPosition({ 0,0,-10 });
	m_editorCamera->aspectRatio = m_renderer->GetAspectRatio();

	// Test gameobjects
	GameObject* kiwi = m_scene->CreateGameobject("kiwi");
	Mesh* kiwiMesh = kiwi->AddComponent<Mesh>();
	kiwiMesh->LoadModel("mesh/kiwi.obj");
	kiwiMesh->renderId = m_renderer->AddMesh(kiwiMesh->vertices, kiwiMesh->indices);
	kiwi->GetComponent<Transform>()->SetPosition( { 5,0,0 });
	kiwi->GetComponent<Transform>()->SetScale({ 0.1,0.1,0.1 });

	GameObject* cube1 = m_scene->CreateGameobject("cube");
	Mesh* mesh1 = cube1->AddComponent<Mesh>();
	mesh1->LoadModel("mesh/cube.obj");
	mesh1->renderId = m_renderer->AddMesh(mesh1->vertices, mesh1->indices);

	// Initialize gameo
	for (GameObject * gameObject : m_scene->GetGameObjects())
		UpdateGameobjectAABB(gameObject, true);

	// Intialization before the main loop
	ImGuiIO& io = ImGui::GetIO();	
	float lastTime = Time::ElapsedSinceStartup();

	// Main loop
	while ( window.WindowOpen() &&  ! m_editorShouldQuit)
	{
		float time = Time::ElapsedSinceStartup();
		float deltaTime = time - lastTime;

		if (deltaTime > m_renderer->framerate.GetDelta())
		{
			// Updates Imgui io and starts new imgui frame
			io.DeltaTime = time - lastTime;
			lastTime = time;
			Input::Update();
			glm::vec2 screenSize = m_renderer->GetSize();
			m_editorCamera->aspectRatio = screenSize.x / screenSize.y;
			io.DisplaySize = ImVec2(screenSize.x, screenSize.y);
			m_editorCamera->Update(deltaTime);

			// camera uniforms
			m_renderer->UpdateCameraUniforms(m_editorCamera->GetProjection(), m_editorCamera->GetView());

			ProcessComponentsModifications();
			
			if (Mouse::KeyDown(Mouse::button0) && !io.WantCaptureMouse)
				SelectGameobject();

			DrawSelectionAABB();

			//Ui
			ImGui::NewFrame();
			RenderGUI();

			m_renderer->DrawFrame();
		}
	}

	delete(m_renderer);
	delete(m_physicsEngine);
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
		m_renderer->RenderGUI();
	
	// Tests window
	if(m_showTestWindow)
		ImGui::ShowTestWindow();

	// Scene Hierarchy
	if (m_showSceneHierarchy)
		m_scene->RenderSceneGui();
	
	// nspector
	if (m_showInspector)
		m_scene->RenderInspectorGui();
}