#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/pipelines/vkDebugPipeline.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "util/fbx/fanFbxImporter.h"
#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "editor/components/fanFPSCamera.h"		
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMesh.h"
#include "scene/components/fanActor.h"

#include "bullet/BulletCollision/CollisionShapes/btCapsuleShape.h"

namespace fan {
	Engine * Engine::ms_engine = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit(false) {

		m_editorGrid.isVisible = true;
		m_editorGrid.color = vk::Color(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 100;
		m_editorGrid.spacing = 1.f;		

		ms_engine = this;

		m_mainMenuBar = new editor::MainMenuBar();
		m_renderWindow = new editor::RenderWindow();
		m_sceneWindow = new editor::SceneWindow();
		m_inspectorWindow = new editor::InspectorWindow();
		m_preferencesWindow = new editor::PreferencesWindow();

		m_renderer = new vk::Renderer({ 1280,720 });
		m_scene = new scene::Scene("mainScene");

		scene::Gameobject * cameraGameobject = m_scene->CreateGameobject("editor_camera");
		cameraGameobject->SetRemovable(false);
		scene::Transform * camTrans = cameraGameobject->AddComponent<scene::Transform>();
		camTrans->SetPosition(btVector3(0, 0, -2));
		scene::Camera * cameraComponent = cameraGameobject->AddComponent<scene::Camera>();
		cameraComponent->SetRemovable(false);
		m_renderer->SetMainCamera(cameraComponent);
		scene::FPSCamera * editorCamera = cameraGameobject->AddComponent<scene::FPSCamera>();
		editorCamera->SetRemovable(false);

		scene::Gameobject * cube = m_scene->CreateGameobject("cube");
		cube->AddComponent<scene::Transform>();
		scene::Mesh * mesh = cube->AddComponent<scene::Mesh>();

		util::FBXImporter importer;
		importer.LoadScene("content/models/test/cube.fbx");
		if (importer.GetMesh(*mesh) == true) {
			m_renderer->AddMesh(mesh);
		}

		cube->GetComponent<scene::Transform>();		
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		delete m_mainMenuBar;
		delete m_renderWindow;
		delete m_sceneWindow;

		delete m_renderer;
		delete m_scene;
	}

	//================================================================================================================================
	//================================================================================================================================

	void Engine::Exit() {
		m_applicationShouldExit = true;
		std::cout << "Exit application" << std::endl;

		for (auto actor : m_startingActors) { m_stoppingActors.insert(actor); }
		m_startingActors.clear();
		for (auto actor : m_activeActors)	{ m_stoppingActors.insert(actor); }
		m_activeActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{
		float lastUpdateTime = Time::ElapsedSinceStartup();
		while ( m_applicationShouldExit == false)
		{
			if (m_renderer->WindowIsOpen() == false) {
				Exit();
			}

			const float time = Time::ElapsedSinceStartup();
			const float updateDelta = time - lastUpdateTime;

			if (updateDelta > 1.f / Time::GetFPS()) {
				lastUpdateTime = time;

				ActorStart();

				for( scene::Actor * actor : m_activeActors ) {
					actor->Update(updateDelta);
				}

				DrawUI();
				DrawEditorGrid();

				m_renderer->DrawFrame();
				m_scene->EndFrame();

				ActorStop();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::AddActor(scene::Actor * _actor) {

		if (m_startingActors.find(_actor) == m_startingActors.end()
			&& m_activeActors.find(_actor) == m_activeActors.end()
			&& m_stoppingActors.find(_actor) == m_stoppingActors.end()) {
			m_startingActors.insert(_actor);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::RemoveActor(scene::Actor * _actor) {
		auto it = m_activeActors.find(_actor);
		if (it != m_activeActors.end()) {
			m_activeActors.erase(_actor);
			m_stoppingActors.insert(_actor);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ActorStart() {
		for ( auto actor : m_startingActors ) {
			actor->Start();	
			m_activeActors.insert(actor);
		}
		m_startingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ActorStop() {
		for (auto actor : m_stoppingActors) {
			actor->Stop();
			delete actor;
		}
		m_stoppingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawEditorGrid() {
		if (m_editorGrid.isVisible == true) {
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for (int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++) {
				m_renderer->DebugLine(btVector3(-count * size, 0.f, coord*size), btVector3(count*size, 0.f, coord*size), m_editorGrid.color);
				m_renderer->DebugLine(btVector3(coord*size, 0.f, -count * size), btVector3(coord*size, 0.f, count*size), m_editorGrid.color);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawUI() {
		m_mainMenuBar->Draw();
		m_renderWindow->Draw();
		m_sceneWindow->Draw();	
		m_inspectorWindow->Draw();
		m_preferencesWindow->Draw();

		// tmp
		static btVector3 pos(0,0,0);
		static btVector3 rot(0, 0, 0);
		static float size = 0.5;

		ImGui::DragFloat3("pos", &pos[0]);
		ImGui::DragFloat3("rot", &rot[0]);
		ImGui::DragFloat("size", &size);

		btQuaternion quat;
		quat.setEulerZYX(rot.x(), rot.y(), rot.z());
		m_renderer->DebugSphere(btTransform(quat, pos), size, 2, vk::Color(1,0,0,1.0f));
	}

}