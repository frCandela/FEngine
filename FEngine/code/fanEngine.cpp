#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/pipelines/vkForwardPipeline.h"
#include "vulkan/pipelines/vkDebugPipeline.h"
#include "vulkan/util/vkShape.h"
#include "vulkan/util/vkWindow.h"
#include "core/fanTime.h"
#include "core/fanInput.h"
#include "core/math/shapes/fanTriangle.h"
#include "core/math/shapes/fanPlane.h"
#include "core/math/shapes/fanAABB.h"
#include "core/files/fanFbxImporter.h"
#include "editor/fanModals.h"
#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "editor/components/fanFPSCamera.h"		
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMesh.h"
#include "scene/components/fanActor.h"
#include "scene/components/fanMesh.h"


#include "bullet/BulletCollision/CollisionShapes/btCapsuleShape.h"

namespace fan {
	Engine * Engine::ms_engine = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		// Get serialized editor values
		m_applicationShouldExit(false),
		m_editorValues("editorValues.json"){

		VkExtent2D windowSize = { 1280,720 };
		m_editorValues.Get("renderer_extent_width", windowSize.width);
		m_editorValues.Get("renderer_extent_height", windowSize.height);

		glm::ivec2 windowPosition = { 0,0 };
		m_editorValues.Get("renderer_position_x", windowPosition.x);
		m_editorValues.Get("renderer_position_y", windowPosition.y);

		// Set some values
		m_editorGrid.isVisible = true;
		m_editorGrid.color = vk::Color(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 10;
		m_editorGrid.spacing = 1.f;		

		// Initialize editor components
		ms_engine = this;
		m_mainMenuBar =			new editor::MainMenuBar();
		m_renderWindow =		new editor::RenderWindow();
		m_sceneWindow =			new editor::SceneWindow();
		m_inspectorWindow =		new editor::InspectorWindow();
		m_preferencesWindow =	new editor::PreferencesWindow();
		m_renderer =			new vk::Renderer(windowSize, windowPosition);
		m_scene = nullptr;

		SetSceneForEditor(new scene::Scene("mainScene"));

		// Sample cube
		scene::Gameobject * cube = m_scene->CreateGameobject("cube");
		cube->AddComponent<scene::Transform>();
		scene::Mesh * mesh = cube->AddComponent<scene::Mesh>();

		mesh->SetPath("content/models/test/cube.fbx");

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
		delete m_scene;
		delete m_renderer;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Exit() {
		m_applicationShouldExit = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{
		float lastUpdateTime = Time::ElapsedSinceStartup();
		while ( m_applicationShouldExit == false && m_renderer->WindowIsOpen() == true)
		{

			const float time = Time::ElapsedSinceStartup();
			const float updateDelta = time - lastUpdateTime;

			if (updateDelta > 1.f / Time::GetFPS()) {
				lastUpdateTime = time;

				m_scene->BeginFrame();

				m_scene->Update(updateDelta);

				ManageSelection();
				DrawUI();
				DrawEditorGrid();

				if (m_mainMenuBar->ShowWireframe()) {
					DrawWireframe();
				}
				if (m_mainMenuBar->ShowAABB()) {
					DrawAABB();
				}

				m_renderer->DrawFrame();
				m_scene->EndFrame();
			}
		}

		// Exit sequence
		std::cout << "Exit application" << std::endl;

	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::SetSceneForEditor(scene::Scene * _scene ) {
		m_renderer->WaitIdle();
		delete m_scene;
		m_scene = _scene;

		m_selectedGameobject = nullptr;

		// Editor Camera
		scene::Gameobject * cameraGameobject = m_scene->CreateGameobject("editor_camera");
		cameraGameobject->SetFlags(scene::Gameobject::NO_DELETE | scene::Gameobject::NOT_SAVED);
		scene::Transform * camTrans = cameraGameobject->AddComponent<scene::Transform>();
		camTrans->SetPosition(btVector3(0, 0, -2));
		m_editorCamera = cameraGameobject->AddComponent<scene::Camera>();
		m_editorCamera->SetRemovable(false);
		m_renderer->SetMainCamera(m_editorCamera);
		scene::FPSCamera * editorCamera = cameraGameobject->AddComponent<scene::FPSCamera>();
		editorCamera->SetRemovable(false);
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
	void Engine::DrawAABB() const {
		const std::vector< scene::Gameobject *>  & gameobjects = m_scene->GetGameObjects();
		for (int gameobjectIndex = 0; gameobjectIndex < gameobjects.size(); gameobjectIndex++) {
			const scene::Gameobject * gameobject = gameobjects[gameobjectIndex];
			if (gameobject != m_editorCamera->GetGameobject()) {
				shape::AABB aabb = gameobject->GetAABB();
				m_renderer->DebugAABB(aabb, vk::Color::Red);
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawWireframe() const {
		const std::vector < vk::MeshData> & meshList = m_renderer->GetMeshList();
		for (int meshIndex = 0; meshIndex < meshList.size(); meshIndex++) {
			const scene::Mesh * const mesh = meshList[meshIndex].mesh;
			const glm::mat4  modelMat = meshList[meshIndex].transform->GetModelMatrix();

const std::vector<uint32_t> & indices = mesh->GetIndices();
const std::vector<vk::Vertex> & vertices = mesh->GetVertices();

for (int index = 0; index < indices.size() / 3; index++) {
	const btVector3 v0 = util::ToBullet(modelMat * glm::vec4(vertices[3 * index + 0].pos, 1.f));
	const btVector3 v1 = util::ToBullet(modelMat * glm::vec4(vertices[3 * index + 1].pos, 1.f));
	const btVector3 v2 = util::ToBullet(modelMat * glm::vec4(vertices[3 * index + 2].pos, 1.f));
	m_renderer->DebugLine(v0, v1, vk::Color::Yellow);
	m_renderer->DebugLine(v1, v2, vk::Color::Yellow);
	m_renderer->DebugLine(v2, v0, vk::Color::Yellow);
}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ManageSelection() {
		bool mouseCaptured = ImGui::GetIO().WantCaptureMouse;;


		// Translation gizmo on selected gameobject
		if (m_selectedGameobject != nullptr && m_selectedGameobject != m_editorCamera->GetGameobject()) {
			scene::Transform * transform = m_selectedGameobject->GetComponent< scene::Transform >();
			btVector3 newPosition;
			if (DrawMoveGizmo(btTransform(btQuaternion(0, 0, 0), transform->GetPosition()), (size_t)this, newPosition)) {
				transform->SetPosition(newPosition);
				mouseCaptured = true;
			}
		}

		// Mouse selection
		if (mouseCaptured == false && Mouse::GetButtonPressed(Mouse::button0)) {

			const btVector3 cameraOrigin = m_editorCamera->GetGameobject()->GetComponent<scene::Transform>()->GetPosition();;
			const shape::Ray ray = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition());
			const std::vector<scene::Gameobject *>  & gameobjects = m_scene->GetGameObjects();

			// Raycast on all the gameobjects
			scene::Gameobject * closestGameobject = nullptr;
			float closestDistance2 = std::numeric_limits<float>::max();
			for (int gameobjectIndex = 0; gameobjectIndex < gameobjects.size(); gameobjectIndex++) {
				scene::Gameobject * gameobject = gameobjects[gameobjectIndex];

				if (gameobject == m_editorCamera->GetGameobject()) {
					continue;
				}

				const shape::AABB & aabb = gameobject->GetAABB();
				btVector3 intersection;
				if (aabb.RayCast(ray.origin, ray.direction, intersection) == true) {
					const float distance2 = intersection.distance2(cameraOrigin);
					if (distance2 < closestDistance2) {
						closestDistance2 = distance2;
						closestGameobject = gameobject;
					}
				}
			}
			SetSelectedGameobject(closestGameobject);
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
	bool Engine::DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition){
		
		GizmoCacheData & cacheData = m_gizmoCacheData[_uniqueID];
		const btVector3 origin = _transform.getOrigin();
		const btTransform rotation(_transform.getRotation());
		const btVector3 axisDirection[3] = { btVector3(1, 0, 0), btVector3(0, 1, 0),  btVector3(0, 0, 1) };
		const btVector3 cameraPosition = m_editorCamera->GetGameobject()->GetComponent<scene::Transform>()->GetPosition();
		const float size = 0.2f * origin.distance(cameraPosition);
		const btTransform coneRotation[3] = {
		btTransform(btQuaternion(0, 0, btRadians(-90)), size*axisDirection[0])
		,btTransform(btQuaternion::getIdentity(),		size*axisDirection[1])
		,btTransform(btQuaternion(0, btRadians(90), 0), size*axisDirection[2])
		};

		_newPosition = _transform.getOrigin();
		for (int axisIndex = 0; axisIndex < 3 ; axisIndex++) 	{
			const vk::Color opaqueColor(axisDirection[axisIndex].x(), axisDirection[axisIndex].y(), axisDirection[axisIndex].z(), 1.f);
			
			// Generates a cone shape
			std::vector<btVector3> coneTris = vk::GetCone(0.1f*size, 0.5f*size, 10);
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
			m_renderer->DebugLine(origin, origin + size*( _transform *  axisDirection[axisIndex] - origin ), opaqueColor);
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

				_newPosition = projectionOnAxis - cacheData.offset;
			}
		}
		return cacheData.pressed;
	}

}