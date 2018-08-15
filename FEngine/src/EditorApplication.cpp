#include "EditorApplication.h"

#include "editor/GameObject.h"
#include "editor/Mesh.h"
#include "util/Time.h"

#include <cmath>

void EditorApplication::ProcessComponentsModifications()
{
	// Iterates over all gameobjects and components
	for ( std::pair<GameObject *, GameObject* > pair : m_scene->GetGameObjects())
	{
		GameObject * gameObject = pair.first;

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
			m_scene->UpdateGameobjectAABB(gameObject, updateAABBGeometry);
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
	for (std::pair<GameObject *, GameObject *> pair : m_scene->GetGameObjects())	
		m_scene->UpdateGameobjectAABB(pair.first, true);	

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
			
			// Clic on the scene
			if (Mouse::KeyDown(Mouse::button0) && !io.WantCaptureMouse)
			{
				// mousepos -> screenspace position
				glm::vec2 ratio = 2.f * Mouse::Position() / m_renderer->GetSize() - glm::vec2(1.f, 1.f);
				ratio.x = std::clamp(ratio.x, -1.f, 1.f);
				ratio.y = std::clamp(ratio.y, -1.f, 1.f);
				
				// Raycast where the user clicked to select a gameobject
				Ray ray = m_editorCamera->ScreenPosToRay(ratio);
				m_scene->SetSelectedGameobject( m_scene->RayCast(ray));
			}

			m_scene->DrawSelectionAABB( *m_renderer);

			//Ui
			ImGui::NewFrame();
			RenderGUI();

			m_renderer->DrawFrame();

			for (GameObject* gameobject : m_scene->GetGameobjectsToDelete())			
				for (Mesh * mesh : gameobject->GetComponents<Mesh>())
					m_renderer->RemoveMesh(mesh->renderId);
				
			m_scene->ApplyDeleteCommands();
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