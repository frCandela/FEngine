#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "util/fbx/fanFbxImporter.h"
#include "editor/fanMainMenuBar.h"
#include "editor/fanRenderWindow.h"	
#include "editor/fanSceneWindow.h"	
#include "editor/fanInspectorWindow.h"	
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMesh.h"

namespace fan {
	Engine * Engine::ms_engine = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit(false) {
		ms_engine = this;

		m_mainMenuBar = new editor::MainMenuBar();
		m_renderWindow = new editor::RenderWindow();
		m_sceneWindow = new editor::SceneWindow();
		m_inspectorWindow = new editor::InspectorWindow();

		m_renderer = new vk::Renderer({ 1280,720 });
		m_scene = new scene::Scene("mainScene");

		scene::Gameobject * camera = m_scene->CreateGameobject("EditorCamera");
		scene::Transform * camTrans = camera->AddComponent<scene::Transform>();
		camTrans->SetPosition(glm::vec3(0, 0, -2));
		scene::Camera * cameraComponent = camera->AddComponent<scene::Camera>();
		cameraComponent->SetRemovable(false);
		m_renderer->SetMainCamera(cameraComponent);


		scene::Gameobject * cube = m_scene->CreateGameobject("cube");
		cube->AddComponent<scene::Transform>();
		scene::Mesh * mesh = cube->AddComponent<scene::Mesh>();

		util::FBXImporter importer;
		importer.LoadScene("mesh/cube.fbx");
		if (importer.GetMesh(*mesh) == true) {
			m_renderer->AddMesh(mesh);
		}
		
		
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
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{
		float lastUpdateTime = Time::ElapsedSinceStartup();
		while (m_renderer->WindowIsOpen() == true && m_applicationShouldExit == false)
		{
			const float time = Time::ElapsedSinceStartup();
			const float updateDelta = time - lastUpdateTime;

			if (updateDelta > 1.f / Time::GetFPS()) {
				lastUpdateTime = time;

				DrawUI();
				m_renderer->DrawFrame();
				m_scene->EndFrame();
			}
		}
	}

	void Engine::DrawUI() {
		m_mainMenuBar->Draw();
		m_renderWindow->Draw();
		m_sceneWindow->Draw();	
		m_inspectorWindow->Draw();
	}
}