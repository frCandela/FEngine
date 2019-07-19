#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/pipelines/vkDebugPipeline.h"
#include "vulkan/util/vkShape.h"
#include "vulkan/util/vkWindow.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "util/shapes/fanTriangle.h"
#include "util/shapes/fanPlane.h"
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
		m_applicationShouldExit(false),
		m_editorValues("editorValues.json"){

		VkExtent2D windowSize = { 1280,720 };
		m_editorValues.Get("renderer_extent_width", windowSize.width);
		m_editorValues.Get("renderer_extent_height", windowSize.height);

		glm::ivec2 windowPosition = { 0,0 };
		m_editorValues.Get("renderer_position_x", windowPosition.x);
		m_editorValues.Get("renderer_position_y", windowPosition.y);

		m_editorGrid.isVisible = true;
		m_editorGrid.color = vk::Color(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 10;
		m_editorGrid.spacing = 1.f;		

		ms_engine = this;

		m_mainMenuBar = new editor::MainMenuBar();
		m_renderWindow = new editor::RenderWindow();
		m_sceneWindow = new editor::SceneWindow();
		m_inspectorWindow = new editor::InspectorWindow();
		m_preferencesWindow = new editor::PreferencesWindow();

		m_renderer = new vk::Renderer(windowSize, windowPosition);
		m_scene = new scene::Scene("mainScene");

		scene::Gameobject * cameraGameobject = m_scene->CreateGameobject("editor_camera");
		cameraGameobject->SetRemovable(false);
		scene::Transform * camTrans = cameraGameobject->AddComponent<scene::Transform>();
		camTrans->SetPosition(btVector3(0, 0, -2));
		m_editorCamera = cameraGameobject->AddComponent<scene::Camera>();
		m_editorCamera->SetRemovable(false);
		m_renderer->SetMainCamera(m_editorCamera);
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


		m_mainMenuBar->Initialize();
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		const vk::Window * window = m_renderer->GetWindow();

		const VkExtent2D rendererSize = window->GetExtent();
		m_editorValues.Set("renderer_extent_width", rendererSize.width);
		m_editorValues.Set("renderer_extent_height", rendererSize.height);

		const glm::ivec2 windowPosition = window->GetPosition();
		m_editorValues.Set("renderer_position_x", windowPosition.x);
		m_editorValues.Set("renderer_position_y", windowPosition.y);

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

				ManageSelection();
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
	void Engine::DrawEditorGrid() const{
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
	void Engine::ManageSelection() {
		// Translation gizmo on selected gameobject
		if (m_selectedGameobject != nullptr && m_selectedGameobject != m_editorCamera->GetGameobject() ) {
			scene::Transform * transform = m_selectedGameobject->GetComponent< scene::Transform >();
			const btVector3 newPosition = DrawMoveGizmo( btTransform( btQuaternion(0,0,0), transform->GetPosition()), (size_t)this);
			transform->SetPosition(newPosition);
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
	}

	//================================================================================================================================
	// Returns the new position of the move gizmo
	// Caller must provide a unique ID to allow proper caching of the user input data
	//================================================================================================================================
	btVector3 Engine::DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID ) {
		
		GizmoCacheData & cacheData = m_gizmoCacheData[_uniqueID];
		const btVector3 origin = _transform.getOrigin();
		const btTransform rotation(_transform.getRotation());
		const float size = 0.1f;
		const btVector3 axisDirection[3] = { btVector3(1, 0, 0), btVector3(0, 1, 0),  btVector3(0, 0, 1) };
		const btTransform coneRotation[3] = {
			btTransform(btQuaternion(0, 0, btRadians(-90)), axisDirection[0])
			,btTransform(btQuaternion::getIdentity(),  axisDirection[1])
			,btTransform(btQuaternion(0, btRadians(90), 0),  axisDirection[2])
		};		

		btVector3 newPosition = _transform.getOrigin();
		for (int axisIndex = 0; axisIndex < 3 ; axisIndex++) 	{
			const vk::Color opaqueColor(axisDirection[axisIndex].x(), axisDirection[axisIndex].y(), axisDirection[axisIndex].z(), 1.f);
			
			// Generates a cone shape
			std::vector<btVector3> coneTris = vk::GetCone(size, 4.f*size, 10);
			btTransform transform = _transform * coneRotation[axisIndex];
			for (int vertIndex = 0; vertIndex < coneTris.size(); vertIndex++) {
				coneTris[vertIndex] = transform * coneTris[vertIndex];
			}

			if (Mouse::GetButtonReleased(Mouse::button0)) {
				cacheData.pressed = false;
				cacheData.axisIndex = -1;
			}


			// Raycast on the gizmo shape to determine if the mouse is hovering it
			vk::Color clickedColor = opaqueColor;
			const shape::Ray ray = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition());
			for (int triIndex = 0; triIndex < coneTris.size() / 3; triIndex++) {
				shape::Triangle triangle(coneTris[3 * triIndex + 0], coneTris[3 * triIndex + 1], coneTris[3 * triIndex + 2]);
				btVector3 intersection;
				if (triangle.RayCast(ray.origin, ray.direction, intersection)) {
					clickedColor[3] = 0.5f;
					if (Mouse::GetButtonPressed(Mouse::button0)) {
						cacheData.pressed = true;
						cacheData.axisIndex = axisIndex;
					}
					break;
				}
			}

			// Draw the gizmo cone & lines
			m_renderer->DebugLine(origin, _transform *  axisDirection[axisIndex], opaqueColor);
			for (int triangleIndex = 0; triangleIndex < coneTris.size() / 3; triangleIndex++) {
				m_renderer->DebugTriangle(coneTris[3 * triangleIndex + 0], coneTris[3 * triangleIndex + 1], coneTris[3 * triangleIndex + 2], clickedColor);
			}

			// Calculate closest point between the mouse ray and the axis selected
			if (cacheData.pressed == true && cacheData.axisIndex == axisIndex ) {
				btVector3 axis = rotation * axisDirection[axisIndex];

				 const shape::Ray screenRay = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition()); 	
				 const shape::Ray axisRay = { origin , axis };				 
				 btVector3 trash, projectionOnAxis;
				 screenRay.RayClosestPoints(axisRay, trash, projectionOnAxis);

				if (Mouse::GetButtonPressed(Mouse::button0)) {
					cacheData.offset = projectionOnAxis - _transform.getOrigin();
				}

				newPosition = projectionOnAxis - cacheData.offset;
			}
		}
		return newPosition;
	}

}