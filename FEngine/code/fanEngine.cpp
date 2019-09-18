#include "fanGlobalIncludes.h"

#include "fanEngine.h"
#include "fanGlobalValues.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/util/fanWindow.h"
#include "core/fanTime.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/math/shapes/fanTriangle.h"
#include "core/math/shapes/fanPlane.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/fanBasicModels.h"
#include "core/files/fanFbxImporter.h"
#include "editor/fanModals.h"
#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "editor/windows/fanConsoleWindow.h"	
#include "editor/components/fanFPSCamera.h"		
#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanActor.h"
#include "scene/components/fanMaterial.h"

#include "core/math/shapes/fanConvexHull.h"

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
		m_editorGrid.color = Color(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 10;
		m_editorGrid.spacing = 1.f;		

		// Initialize editor components
		ms_engine = this;
		m_mainMenuBar =			new editor::MainMenuBar();
		m_renderWindow =		new editor::RenderWindow();
		m_sceneWindow =			new editor::SceneWindow();
		m_inspectorWindow =		new editor::InspectorWindow();
		m_preferencesWindow =	new editor::PreferencesWindow();
		m_consoleWindow =		new editor::ConsoleWindow();
		Renderer::Get().Initialize(windowSize, windowPosition);
		m_scene =				new scene::Scene("mainScene");
		m_scene->onSceneLoad.Connect(&Engine::OnSceneLoad, this);
		m_scene->New();

		scene::Scene::onSceneClear.Connect			( &Renderer::Clear,					&Renderer::Get());
		scene::Material::onMaterialAttach.Connect	( &Renderer::RegisterMaterial,		&Renderer::Get() );
		scene::Material::onMaterialDetach.Connect	( &Renderer::UnRegisterMaterial,	&Renderer::Get());		
		scene::Model::onRegisterModel.Connect		( &Renderer::RegisterModel,			&Renderer::Get());
		scene::Model::onUnRegisterModel.Connect		( &Renderer::UnRegisterModel,		&Renderer::Get());

		m_mainMenuBar->Initialize();

		fan::Mesh * defaultMesh = Renderer::Get().GetRessourceManager()->LoadMesh(GlobalValues::s_defaultMeshPath);
		Renderer::Get().GetRessourceManager()->SetDefaultMesh( defaultMesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		const Window * window = Renderer::Get().GetWindow();

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
		Renderer::Get().Destroy();
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
		while ( m_applicationShouldExit == false && Renderer::Get().WindowIsOpen() == true)
		{
			const float time = Time::ElapsedSinceStartup();
			const float delta = Time::GetDelta();
			float updateDelta = time - lastUpdateTime;

			if ( updateDelta > delta ) {
				if (updateDelta > 2 * delta) {
					Debug::Get() << Debug::Severity::warning << "Lag detected, delta = " << (int)(1000*updateDelta) << "ms." << Debug::Endl();
				}
				updateDelta -= delta;
				lastUpdateTime = time;

				m_scene->BeginFrame();
				m_scene->Update( delta );

				ManageSelection();
				DrawUI();
				DrawEditorGrid();

				if (m_mainMenuBar->ShowWireframe()) { DrawWireframe();	}
				if (m_mainMenuBar->ShowNormals())	{ DrawNormals();	}
				if (m_mainMenuBar->ShowAABB())		{ DrawAABB();		}
				if (m_mainMenuBar->ShowHull())		{ DrawHull();		}

				Renderer::Get().DrawFrame();
				m_scene->EndFrame();
			}
		}

		// Exit sequence
		fan::Debug::Log( "Exit application" );

	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnSceneLoad(scene::Scene * _scene) {

		m_selectedentity = nullptr;

		// Editor Camera
		scene::Entity * cameraentity = _scene->CreateEntity("editor_camera");
		cameraentity->SetFlags(scene::Entity::NO_DELETE | scene::Entity::NOT_SAVED);
		scene::Transform * camTrans = cameraentity->AddComponent<scene::Transform>();
		camTrans->SetPosition(btVector3(0, 0, -2));
		m_editorCamera = cameraentity->AddComponent<scene::Camera>();
		m_editorCamera->SetRemovable(false);
		Renderer::Get().SetMainCamera(m_editorCamera);
		scene::FPSCamera * editorCamera = cameraentity->AddComponent<scene::FPSCamera>();
		editorCamera->SetRemovable(false);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawEditorGrid() const {
		if (m_editorGrid.isVisible == true) {
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for (int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++) {
				Renderer::Get().DebugLine(btVector3(-count * size, 0.f, coord*size), btVector3(count*size, 0.f, coord*size), m_editorGrid.color);
				Renderer::Get().DebugLine(btVector3(coord*size, 0.f, -count * size), btVector3(coord*size, 0.f, count*size), m_editorGrid.color);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawAABB() const {
		const std::vector< scene::Entity *>  & entities = m_scene->BuildEntitiesList();
		for (int entityIndex = 0; entityIndex < entities.size(); entityIndex++) {
			const scene::Entity * entity = entities[entityIndex];
			if (entity != m_editorCamera->GetEntity()) {
				shape::AABB aabb = entity->GetAABB();
				Renderer::Get().DebugAABB(aabb, Color::Red);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawHull() const	{
		if (m_selectedentity != nullptr) {
			scene::Model * model = m_selectedentity->GetComponent<scene::Model>();
			if (model != nullptr) {
				const shape::ConvexHull * hull = nullptr;
				Mesh * mesh = model->GetMesh();
				if (mesh != nullptr) {
					hull = mesh->GetConvexHull();
				}
				if (hull != nullptr) {
					const std::vector<btVector3> & vertices = hull->GetVertices();
					const std::vector<uint32_t> & indices = hull->GetIndices();
					if (!vertices.empty()) {

						int counthull = (int)vertices.size();
						ImGui::DragInt("hull vertices size", &counthull);
						int countidxhull = (int)indices.size();
						ImGui::DragInt("hull indices size", &countidxhull);

						Color color(1, 0, 0, 1);
						for (unsigned polyIndex = 0; polyIndex < indices.size() / 3; polyIndex++) {
							const int index0 = indices[3 * polyIndex + 0];
							const int index1 = indices[3 * polyIndex + 1];
							const int index2 = indices[3 * polyIndex + 2];
							const btVector3 vec0 = vertices[index0];
							const btVector3 vec1 = vertices[index1];
							const btVector3 vec2 = vertices[index2];
							
							Renderer::Get().DebugLine(vec0, vec1, color);
							Renderer::Get().DebugLine(vec1, vec2, color);
							Renderer::Get().DebugLine(vec2, vec0, color);
		
						}
					}
				}
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawWireframe() const {
		const std::vector < vk::DrawData> & modelList = Renderer::Get().GetDrawData();
		for (int meshIndex = 0; meshIndex < modelList.size(); meshIndex++) {
			const fan::Mesh* mesh = modelList[meshIndex].model->GetMesh();
			const scene::Model * model = modelList[meshIndex].model;

			const glm::mat4  modelMat = model->GetEntity()->GetComponent<scene::Transform>()->GetModelMatrix();

			const std::vector<uint32_t> & indices = mesh->GetIndices();
			const std::vector<vk::Vertex> & vertices = mesh->GetVertices();

			for (int index = 0; index < indices.size() / 3; index++) {
				const btVector3 v0 = ToBullet(modelMat * glm::vec4(vertices[3 * index + 0].pos, 1.f));
				const btVector3 v1 = ToBullet(modelMat * glm::vec4(vertices[3 * index + 1].pos, 1.f));
				const btVector3 v2 = ToBullet(modelMat * glm::vec4(vertices[3 * index + 2].pos, 1.f));
				Renderer::Get().DebugLine(v0, v1, Color::Yellow);
				Renderer::Get().DebugLine(v1, v2, Color::Yellow);
				Renderer::Get().DebugLine(v2, v0, Color::Yellow);
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawNormals() const {
		const std::vector < vk::DrawData> & modelList = Renderer::Get().GetDrawData();
		for (int meshIndex = 0; meshIndex < modelList.size(); meshIndex++) {
			const fan::Mesh* mesh = modelList[meshIndex].model->GetMesh();
			const scene::Model * model = modelList[meshIndex].model;

			const glm::mat4  modelMat = model->GetEntity()->GetComponent<scene::Transform>()->GetModelMatrix();
			const glm::mat4  rotationMat = model->GetEntity()->GetComponent<scene::Transform>()->GetRotationMat();

			const std::vector<uint32_t> & indices = mesh->GetIndices();
			const std::vector<vk::Vertex> & vertices = mesh->GetVertices();

			for (int index = 0; index < indices.size(); index++) {
				const btVector3 vertex = ToBullet( modelMat * glm::vec4(vertices[index].pos, 1.f));
				const btVector3 normal = ToBullet(rotationMat * glm::vec4(vertices[index].normal, 1.f));
				Renderer::Get().DebugLine(vertex, vertex + 0.1f * normal, Color::Red);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ManageSelection() {
		bool mouseCaptured = ImGui::GetIO().WantCaptureMouse;

		// Translation gizmo on selected entity
		if (m_selectedentity != nullptr && m_selectedentity != m_editorCamera->GetEntity()) {
			scene::Transform * transform = m_selectedentity->GetComponent< scene::Transform >();
			btVector3 newPosition;
			if (DrawMoveGizmo(btTransform(btQuaternion(0, 0, 0), transform->GetPosition()), (size_t)this, newPosition)) {
				transform->SetPosition(newPosition);
				mouseCaptured = true;
			}
		}

		// Mouse selection
		if (mouseCaptured == false && Mouse::GetButtonPressed(Mouse::button0)) {

			const btVector3 cameraOrigin = m_editorCamera->GetEntity()->GetComponent<scene::Transform>()->GetPosition();;
			const shape::Ray ray = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition());
			const std::vector<scene::Entity *>  & entities = m_scene->BuildEntitiesList();

			// Raycast on all the entities
			scene::Entity * closestentity = nullptr;
			float closestDistance2 = std::numeric_limits<float>::max();
			for (int entityIndex = 0; entityIndex < entities.size(); entityIndex++) {
				scene::Entity * entity = entities[entityIndex];

				if (entity == m_editorCamera->GetEntity()) {
					continue;
				}

				const shape::AABB & aabb = entity->GetAABB();
				btVector3 intersection;
				if (aabb.RayCast(ray.origin, ray.direction, intersection) == true) {
					const float distance2 = intersection.distance2(cameraOrigin);
					if (distance2 < closestDistance2) {
						closestDistance2 = distance2;
						closestentity = entity;
					}
				}
			}
			SetSelectedEntity(closestentity);
		}
	}	

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawUI() {

		//***************************************************************************************MYLITTLESPACE


		ImGui::Begin("test"); {

		} ImGui::End();

		//***************************************************************************************END_MYLITTLESPACE

		m_mainMenuBar->Draw();
		m_renderWindow->Draw();
		m_sceneWindow->Draw();
		m_inspectorWindow->Draw();
		m_preferencesWindow->Draw();
		m_consoleWindow->Draw();
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
		const btVector3 cameraPosition = m_editorCamera->GetEntity()->GetComponent<scene::Transform>()->GetPosition();
		const float size = 0.2f * origin.distance(cameraPosition);
		const btTransform coneRotation[3] = {
		btTransform(btQuaternion(0, 0, btRadians(-90)), size*axisDirection[0])
		,btTransform(btQuaternion::getIdentity(),		size*axisDirection[1])
		,btTransform(btQuaternion(0, btRadians(90), 0), size*axisDirection[2])
		};

		_newPosition = _transform.getOrigin();
		for (int axisIndex = 0; axisIndex < 3 ; axisIndex++) 	{
			const Color opaqueColor(axisDirection[axisIndex].x(), axisDirection[axisIndex].y(), axisDirection[axisIndex].z(), 1.f);
			
			// Generates a cone shape
			std::vector<btVector3> coneTris = GetCone(0.1f*size, 0.5f*size, 10);
			btTransform transform = _transform * coneRotation[axisIndex];
			for (int vertIndex = 0; vertIndex < coneTris.size(); vertIndex++) {
				coneTris[vertIndex] = transform * coneTris[vertIndex];
			}

			if (Mouse::GetButtonReleased(Mouse::button0)) {
				cacheData.pressed = false;
				cacheData.axisIndex = -1;
			}

			// Raycast on the gizmo shape to determine if the mouse is hovering it
			Color clickedColor = opaqueColor;
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
			Renderer::Get().DebugLine(origin, origin + size*( _transform *  axisDirection[axisIndex] - origin ), opaqueColor, false );
			for (int triangleIndex = 0; triangleIndex < coneTris.size() / 3; triangleIndex++) {
				Renderer::Get().DebugTriangle(coneTris[3 * triangleIndex + 0], coneTris[3 * triangleIndex + 1], coneTris[3 * triangleIndex + 2], clickedColor);
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