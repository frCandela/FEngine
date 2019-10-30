#include "fanGlobalIncludes.h"
#include "fanEngine.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/util/fanWindow.h"
#include "renderer/core/fanTexture.h"
#include "core/time/fanTime.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/input/fanInputManager.h"
#include "core/math/shapes/fanTriangle.h"
#include "core/math/shapes/fanPlane.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/fanBasicModels.h"
#include "core/files/fanFbxImporter.h"
#include "ecs/fanECSManager.h"
#include "editor/fanModals.h"
#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "editor/windows/fanConsoleWindow.h"	
#include "editor/windows/fanEcsWindow.h"	
#include "editor/windows/fanProfilerWindow.h"	
#include "editor/components/fanFPSCamera.h"		
#include "editor/fanImguiIcons.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanModel.h"
#include "scene/actors/fanActor.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanDirectionalLight.h"
#include "scene/components/fanRigidbody.h"
#include "scene/components/fanSphereShape.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/time/fanProfiler.h"
#include "ecs/fanECSManager.h"
#include "physics/fanPhysicsManager.h"

namespace fan {
	Signal<Gameobject*> Engine::onGameobjectSelected;
	Signal<Camera*> Engine::onSetCamera;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit(false),
		m_mainCamera(nullptr)
	{

		// Get serialized editor values
		VkExtent2D windowSize = { 1280,720 };
		SerializedValues::Get().GetUInt("renderer_extent_width", windowSize.width);
		SerializedValues::Get().GetUInt("renderer_extent_height", windowSize.height);

		glm::ivec2 windowPosition = { 0,23 };
		SerializedValues::Get().GetInt("renderer_position_x", windowPosition.x);
		SerializedValues::Get().GetInt("renderer_position_y", windowPosition.y);

		SerializedValues::Get().LoadKeyBindings();

		// Creates keyboard events
		Input::Get().Manager().CreateKeyboardEvent( "delete",		  Keyboard::DELETE	);
		Input::Get().Manager().CreateKeyboardEvent( "reload_shaders", Keyboard::F5		);
		Input::Get().Manager().CreateKeyboardEvent( "open_scene",	  Keyboard::O, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "save_scene",	  Keyboard::S, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "reload_scene",	  Keyboard::R, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent(	"play_pause",	  Keyboard::TAB );
		Input::Get().Manager().CreateKeyboardEvent( "freeze_capture", Keyboard::END );

		// Axis
		Input::Get().Manager().CreateAxis( "game_forward",		Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateAxis( "game_left",			Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateAxis( "editor_forward",	Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateAxis( "editor_left",		Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateAxis( "editor_up",			Keyboard::E, Keyboard::Q );
		Input::Get().Manager().CreateAxis( "editor_boost",		Keyboard::LEFT_SHIFT, Keyboard::NONE );

		// Set some values
		m_editorGrid.isVisible = true;
		m_editorGrid.color = Color(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 10;
		m_editorGrid.spacing = 1.f;		

		m_renderer = new Renderer( windowSize, windowPosition );
		m_ecsManager = new EcsManager();
		m_physicsManager = new PhysicsManager( btVector3( 0, 0, 0 ) );
		m_scene = new Scene( "mainScene", m_ecsManager, m_physicsManager );

		// Initialize editor components
		m_renderWindow		= new RenderWindow( m_renderer );
		m_sceneWindow		= new SceneWindow( m_scene );
		m_inspectorWindow	= new InspectorWindow();
		m_preferencesWindow = new PreferencesWindow( m_renderer );
		m_consoleWindow		= new ConsoleWindow();
		m_ecsWindow			= new EcsWindow( m_ecsManager );
		m_profilerWindow	= new ProfilerWindow( );
		m_mainMenuBar		= new MainMenuBar( *m_scene, m_editorGrid );
		m_mainMenuBar->SetWindows( { m_renderWindow , m_sceneWindow , m_inspectorWindow , m_consoleWindow, m_ecsWindow, m_profilerWindow, m_preferencesWindow } );

		// Instance messages		
		Debug::Get().SetDebug( m_renderer );
		m_sceneWindow->onSelectGameobject.Connect( &Engine::SetSelectedGameobject, this );
		m_mainMenuBar->onReloadShaders.Connect(&Renderer::ReloadShaders, m_renderer );
		m_mainMenuBar->onExit.Connect( &Engine::Exit, this );
		onGameobjectSelected.Connect( &SceneWindow::OnGameobjectSelected, m_sceneWindow );
		onGameobjectSelected.Connect( &InspectorWindow::OnGameobjectSelected, m_inspectorWindow );
		m_scene->onSceneLoad.Connect( &SceneWindow::OnSceneLoad, m_sceneWindow );
		m_scene->onSceneLoad.Connect( &Engine::OnSceneLoad, this );
		m_scene->onSceneClear.Connect  ( &Renderer::Clear, m_renderer );
		m_scene->onDeleteGameobject.Connect( &Engine::OnGameobjectDeleted, this );

		// Events linking
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect(	&Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "delete" )->Connect(			&Engine::DeleteSelection, this );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect(		&Engine::SwitchPlayPause, this );

		// Static messages		
		Material::onMaterialSetPath.Connect		( &Engine::OnMaterialSetTexture, this );
		Mesh::s_onMeshLoad.Connect				( &RessourceManager::OnLoadMesh, m_renderer->GetRessourceManager() );
		Mesh::s_onMeshDelete.Connect			( &Renderer::WaitIdle, m_renderer );
		Model::onModelSetPath.Connect			( &Engine::OnModelSetPath,		 this );
		Model::onRegisterModel.Connect			( &Engine::RegisterModel,		 this );
		Model::onUnRegisterModel.Connect		( &Engine::UnRegisterModel,		 this );
		PointLight::onPointLightAttach.Connect	( &Engine::RegisterPointLight,	 this );
		PointLight::onPointLightDetach.Connect	( &Engine::UnRegisterPointLight, this );
		DirectionalLight::onDirectionalLightAttach.Connect	( &Engine::RegisterDirectionalLight,   this );
		DirectionalLight::onDirectionalLightDetach.Connect	( &Engine::UnRegisterDirectionalLight, this );

		m_scene->New();
		Mesh * defaultMesh = m_renderer->GetRessourceManager()->LoadMesh(GlobalValues::s_defaultMesh);
		m_renderer->GetRessourceManager()->SetDefaultMesh( defaultMesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		// Deletes ui
		delete m_mainMenuBar;
		delete m_scene;
		delete m_physicsManager;
		delete m_ecsManager;

		// Serialize editor positions
		const Window * window = m_renderer->GetWindow();
		const VkExtent2D rendererSize = window->GetExtent();
		const glm::ivec2 windowPosition = window->GetPosition();
		SerializedValues::Get().SetUInt("renderer_extent_width", rendererSize.width);
		SerializedValues::Get().SetUInt("renderer_extent_height", rendererSize.height);
		SerializedValues::Get().SetInt("renderer_position_x", windowPosition.x);
		SerializedValues::Get().SetInt("renderer_position_y", windowPosition.y);
		SerializedValues::Get().SaveValuesToDisk();

		delete ( m_renderer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Exit() {
		m_applicationShouldExit = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run() {

		Clock logicClock;
		Clock renderClock;

		Profiler::Get().Begin();

		while ( m_applicationShouldExit == false && m_renderer->WindowIsOpen() == true ) 
		{
 			const float time = Time::ElapsedSinceStartup();

			// Runs logic, renders ui
			const float targetLogicDelta = Time::Get().GetLogicDelta();
			if ( logicClock.ElapsedSeconds() > targetLogicDelta ) {
				logicClock.Reset();

				SCOPED_PROFILE(logic)
				{
					SCOPED_PROFILE( init )
					Input::Get().NewFrame();
					ImGui::NewFrame();
					ImGui::GetIO().DeltaTime = targetLogicDelta;
					m_renderer->ClearDebug();
				}

				m_scene->BeginFrame();
				m_scene->Update( targetLogicDelta );
				m_ecsManager->Update( targetLogicDelta, m_scene->GetMainCamera()->GetGameobject()->GetTransform()->GetPosition() );				
				m_physicsManager->StepSimulation(targetLogicDelta);
				m_ecsManager->LateUpdate( targetLogicDelta );

				m_ecsManager->Refresh();

				{
					SCOPED_PROFILE( draw_ui )
					ManageSelection();
					m_mainMenuBar->Draw();
					m_physicsManager->OnGui();
					DrawEditorGrid();
					Input::Get().Manager().PullEvents();
				}

				{
					SCOPED_PROFILE( debug_draw )
					if ( m_mainMenuBar->ShowWireframe() ) { DrawWireframe(); }
					if ( m_mainMenuBar->ShowNormals() ) { DrawNormals(); }
					if ( m_mainMenuBar->ShowAABB() ) { DrawAABB(); }
					if ( m_mainMenuBar->ShowHull() ) { DrawHull(); }
				}

				m_scene->EndFrame();	

				{
					SCOPED_PROFILE( imgui_render )
					ImGui::Render();
				}
			}

			// Render world
			const float targetRenderDelta = Time::Get().GetRenderDelta();
			if ( renderClock.ElapsedSeconds() > targetRenderDelta ) {
				renderClock.Reset();
				Time::Get().RegisterFrameDrawn();	// used for stats
				UpdateRenderer();
				
				m_renderer->DrawFrame();		
				Profiler::Get().End();
				Profiler::Get().Begin();
			} 
		}

		// Exit sequence
		Debug::Log( "Exit application" );
	}
	
	//================================================================================================================================
	// Todo save camera postion depending on the scene
	//================================================================================================================================
	void Engine::OnSceneLoad(Scene * _scene) {

		m_selectedGameobject = nullptr;

		// Editor Camera
		Gameobject * cameraGameobject = _scene->CreateGameobject("editor_camera");
		cameraGameobject->SetFlags( Gameobject::Flag::NO_DELETE | Gameobject::Flag::NOT_SAVED );

		cameraGameobject->GetTransform()->SetPosition(btVector3(0, 0, -2));
		m_editorCamera = cameraGameobject->AddComponent<Camera>();
		m_editorCamera->SetRemovable(false);
		SetMainCamera(m_editorCamera);
		m_editorCameraController = cameraGameobject->AddComponent<FPSCamera>();
		m_editorCameraController->SetRemovable(false);

		_scene->onSetMainCamera.Connect( &Engine::SetMainCamera, this );

		Debug::Get().SetDebug( m_renderer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawEditorGrid() const {
		if (m_editorGrid.isVisible == true) {
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for (int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++) {
				Debug::Render().DebugLine(btVector3(-count * size, 0.f, coord*size), btVector3(count*size, 0.f, coord*size), m_editorGrid.color);
				Debug::Render().DebugLine(btVector3(coord*size, 0.f, -count * size), btVector3(coord*size, 0.f, count*size), m_editorGrid.color);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawAABB() const {
		const std::vector< Gameobject *>  & entities = m_scene->BuildEntitiesList();
		for (int gameobjectIndex = 0; gameobjectIndex < entities.size(); gameobjectIndex++) {
			const Gameobject * gameobject = entities[gameobjectIndex];
			if (gameobject != m_editorCamera->GetGameobject()) {
				AABB aabb = gameobject->GetAABB();
				m_renderer->DebugAABB(aabb, Color::Red);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawHull() const	{
		if (m_selectedGameobject != nullptr) {
			Model * model = m_selectedGameobject->GetComponent<Model>();
			if (model != nullptr) {
				const ConvexHull * hull = nullptr;
				Mesh * mesh = model->GetMesh();
				if (mesh != nullptr) {
					hull = & model->GetConvexHull();
				}
				if (hull != nullptr) {
					const std::vector<btVector3> & vertices = hull->GetVertices();
					const std::vector<uint32_t> & indices = hull->GetIndices();
					if (!vertices.empty()) {
						const glm::mat4  modelMat = model->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();

						Color color = Color::Cyan;
						for (unsigned polyIndex = 0; polyIndex < indices.size() / 3; polyIndex++) {
							const int index0 = indices[3 * polyIndex + 0];
							const int index1 = indices[3 * polyIndex + 1];
							const int index2 = indices[3 * polyIndex + 2];
							const btVector3 vec0 = vertices[index0];
							const btVector3 vec1 = vertices[index1];
							const btVector3 vec2 = vertices[index2];
							const btVector3 worldVec0 = ToBullet(modelMat * glm::vec4(vec0[0], vec0[1], vec0[2], 1.f));
							const btVector3 worldVec1 = ToBullet(modelMat * glm::vec4(vec1[0], vec1[1], vec1[2], 1.f));
							const btVector3 worldVec2 = ToBullet(modelMat * glm::vec4(vec2[0], vec2[1], vec2[2], 1.f));
							
							Debug::Render().DebugLine(worldVec0, worldVec1, color);
							Debug::Render().DebugLine(worldVec1, worldVec2, color);
							Debug::Render().DebugLine(worldVec2, worldVec0, color);
		
						}
					}
				}
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawWireframe() const {
		if (m_selectedGameobject != nullptr) {
			Model * model = m_selectedGameobject->GetComponent<Model>();
			if (model != nullptr) {
				Mesh * mesh = model->GetMesh();
				if (mesh != nullptr) {
					const glm::mat4  modelMat = model->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
					const std::vector<uint32_t> & indices = mesh->GetIndices();
					const std::vector<Vertex> & vertices = mesh->GetVertices();

					for (int index = 0; index < indices.size() / 3; index++) {
						const btVector3 v0 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 0]].pos, 1.f));
						const btVector3 v1 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 1]].pos, 1.f));
						const btVector3 v2 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 2]].pos, 1.f));
						Debug::Render().DebugLine(v0, v1, Color::Yellow);
						Debug::Render().DebugLine(v1, v2, Color::Yellow);
						Debug::Render().DebugLine(v2, v0, Color::Yellow);
					}
				}
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawNormals() const {

		if (m_selectedGameobject != nullptr) {
			Model * model = m_selectedGameobject->GetComponent<Model>();
			if (model != nullptr) {
				Mesh * mesh = model->GetMesh();
				if (mesh != nullptr) {
					const glm::mat4  modelMat = model->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
					const glm::mat4  normalMat = model->GetGameobject()->GetComponent<Transform>()->GetNormalMatrix();
					const std::vector<uint32_t> & indices = mesh->GetIndices();
					const std::vector<Vertex> & vertices = mesh->GetVertices();

					for (int index = 0; index < indices.size(); index++) {
						const Vertex& vertex = vertices[indices[index]];
						const btVector3 position = ToBullet(modelMat * glm::vec4(vertex.pos, 1.f));
						const btVector3 normal = ToBullet(normalMat * glm::vec4(vertex.normal, 1.f));
						Debug::Render().DebugLine(position, position + 0.1f * normal, Color::Green);
					}
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SetMainCamera( Camera * _mainCamera ) { 
		if ( _mainCamera != m_mainCamera ) {
			m_mainCamera = _mainCamera;
			m_scene->SetMainCamera(_mainCamera);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SetSelectedGameobject( Gameobject * _selectedGameobject ) {
		m_selectedGameobject = _selectedGameobject; 
		onGameobjectSelected.Emmit( m_selectedGameobject );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Deselect() { 
		m_selectedGameobject = nullptr; 
		onGameobjectSelected.Emmit( m_selectedGameobject );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SwitchPlayPause() {
		if ( m_scene->IsPaused() ) {
			m_scene->Play();
			m_editorCameraController->SetEnabled( false );
		} else {
			m_scene->Pause();
			SetMainCamera( m_editorCamera );
			m_editorCameraController->SetEnabled(true);
		}
	}

	//================================================================================================================================
	// TODO use an ecs system to do this
	//================================================================================================================================
	void Engine::UpdateRenderer() {
		SCOPED_PROFILE( update_renderer )

		// Camera
		Transform * cameraTransform = m_mainCamera->GetGameobject()->GetComponent<Transform>();
		m_mainCamera->SetAspectRatio( m_renderer->GetWindowAspectRatio() );
		m_renderer->SetMainCamera( m_mainCamera->GetProjection(), m_mainCamera->GetView(), ToGLM(cameraTransform->GetPosition()));		

		// Point lights		
		for ( int lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++ ) {
			const PointLight * light = m_pointLights[lightIndex];
			const Transform *  lightTransform = light->GetGameobject()->GetComponent<Transform>();
			m_renderer->SetPointLight(
				lightIndex,
				ToGLM( lightTransform->GetPosition() ),
				light->GetDiffuse().ToGLM(),
				light->GetSpecular().ToGLM(),
				light->GetAmbiant().ToGLM(),
				light->GetAttenuation()
			);
		}	m_renderer->SetNumPointLights( static_cast<uint32_t>( m_pointLights.size() ) );
		
		// Directional lights		
		for ( int lightIndex = 0; lightIndex < m_directionalLights.size(); lightIndex++ ) {
			const DirectionalLight * light			= m_directionalLights[lightIndex];
			const Transform *		 lightTransform = light->GetGameobject()->GetComponent<Transform>();
			m_renderer->SetDirectionalLight( 
				lightIndex
				,glm::vec4( ToGLM( lightTransform->Forward() ), 1 )
				,light->GetAmbiant().ToGLM()
				,light->GetDiffuse().ToGLM()
				,light->GetSpecular().ToGLM()
			);
		} m_renderer->SetNumDirectionalLights( static_cast<uint32_t>( m_directionalLights.size() ) );

		// Transforms, mesh, materials
		std::vector<DrawData> drawData( m_models.size() );
		for (int modelIndex = 0; modelIndex < m_models.size() ; modelIndex++) {

			DrawData& data = drawData[modelIndex];
			Model * model = m_models[modelIndex];
			Transform * transform = model->GetGameobject()->GetTransform();
			Material * material = model->GetGameobject()->GetComponent<Material>();

			// Mesh
			data.mesh = model->GetMesh();

			// Transform
			data.modelMatrix = transform->GetModelMatrix();
			data.normalMatrix = transform->GetNormalMatrix();			

			// Materials
			if ( material != nullptr ) {
				const uint32_t textureIndex = material->GetTexture() != nullptr ? material->GetTexture()->GetRenderID() : 0;
				data.color = material->GetColor().ToGLM();
				data.shininess = material->GetShininess();
				data.textureIndex = textureIndex;
			} else {
				data.color = Color::White.ToGLM();
				data.shininess = 1;
				data.textureIndex = 0;
			}
		}
		m_renderer->SetDrawData(drawData);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DeleteSelection() {
		if ( m_selectedGameobject != nullptr ) {
			m_scene->DeleteGameobject( m_selectedGameobject );
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ManageSelection() {
		SCOPED_PROFILE( selection )

		bool mouseCaptured = ImGui::GetIO().WantCaptureMouse;

		// Translation gizmo on selected gameobject
		if (m_selectedGameobject != nullptr && m_selectedGameobject != m_editorCamera->GetGameobject()) {
			Transform * transform = m_selectedGameobject->GetComponent< Transform >();
			btVector3 newPosition;
			if (DrawMoveGizmo(btTransform(btQuaternion(0, 0, 0), transform->GetPosition()), (size_t)this, newPosition)) {
				transform->SetPosition(newPosition);
				mouseCaptured = true;
			}
		}

		// Mouse selection
		if (mouseCaptured == false && Mouse::GetButtonPressed(Mouse::button0)) {
			const btVector3 cameraOrigin = m_editorCamera->GetGameobject()->GetComponent<Transform>()->GetPosition();;
			const Ray ray = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition());
			const std::vector<Gameobject *>  & entities = m_scene->BuildEntitiesList();

			// Raycast on all the entities
			Gameobject * closestGameobject = nullptr;
			float closestDistance2 = std::numeric_limits<float>::max();
			for (int gameobjectIndex = 0; gameobjectIndex < entities.size(); gameobjectIndex++) {
				Gameobject * gameobject = entities[gameobjectIndex];

				if (gameobject == m_editorCamera->GetGameobject()) {
					continue;
				}

				const AABB & aabb = gameobject->GetAABB();
				btVector3 intersection;
				if (aabb.RayCast(ray.origin, ray.direction, intersection) == true) {
					Model * model = gameobject->GetComponent<Model>();
					if (model != nullptr && model->GetMesh() != nullptr ) {
						Transform * transform = gameobject->GetComponent<Transform>();
						const Ray transformedRay(transform->InverseTransformPoint(ray.origin), transform->InverseTransformDirection(ray.direction));
						if (model->GetConvexHull().RayCast(transformedRay.origin, transformedRay.direction, intersection) == false) {
							continue;
						}
					}
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
	// Returns the new position of the move gizmo
	// Caller must provide a unique ID to allow proper caching of the user input data
	//================================================================================================================================
	bool Engine::DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition){
		
		GizmoCacheData & cacheData = m_gizmoCacheData[_uniqueID];
		const btVector3 origin = _transform.getOrigin();
		const btTransform rotation(_transform.getRotation());
		const btVector3 axisDirection[3] = { btVector3(1, 0, 0), btVector3(0, 1, 0),  btVector3(0, 0, 1) };
		const btVector3 cameraPosition = m_editorCamera->GetGameobject()->GetComponent<Transform>()->GetPosition();
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

			if ( ImGui::IsMouseReleased(0)) {
				cacheData.pressed = false;
				cacheData.axisIndex = -1;
			}

			// Raycast on the gizmo shape to determine if the mouse is hovering it
			Color clickedColor = opaqueColor;
			const Ray ray = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition());
			for (int triIndex = 0; triIndex < coneTris.size() / 3; triIndex++) {
				Triangle triangle(coneTris[3 * triIndex + 0], coneTris[3 * triIndex + 1], coneTris[3 * triIndex + 2]);
				btVector3 intersection;
				if (triangle.RayCast(ray.origin, ray.direction, intersection)) {
					clickedColor[3] = 0.5f;
					if (Mouse::GetButtonPressed(0)) {
						cacheData.pressed = true;
						cacheData.axisIndex = axisIndex;
					}
					break;
				}
			}

			// Draw the gizmo cone & lines
			Debug::Render().DebugLine(origin, origin + size*( _transform *  axisDirection[axisIndex] - origin ), opaqueColor, false );
			for (int triangleIndex = 0; triangleIndex < coneTris.size() / 3; triangleIndex++) {
				m_renderer->DebugTriangle(coneTris[3 * triangleIndex + 0], coneTris[3 * triangleIndex + 1], coneTris[3 * triangleIndex + 2], clickedColor);
			}

			// Calculate closest point between the mouse ray and the axis selected
			if (cacheData.pressed == true && cacheData.axisIndex == axisIndex ) {
				btVector3 axis = rotation * axisDirection[axisIndex];

				 const Ray screenRay = m_editorCamera->ScreenPosToRay(Mouse::GetScreenSpacePosition()); 	
				 const Ray axisRay = { origin , axis };				 
				 btVector3 trash, projectionOnAxis;
				 screenRay.RayClosestPoints(axisRay, trash, projectionOnAxis);

				if ( Mouse::GetButtonPressed( 0 ) ) {
					cacheData.offset = projectionOnAxis - _transform.getOrigin();
				}

				_newPosition = projectionOnAxis - cacheData.offset;
			}
		}
		return cacheData.pressed;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::RegisterDirectionalLight( DirectionalLight * _directionalLight ) {

		// Looks for the _directionalLight
		for ( int lightIndex = 0; lightIndex < m_directionalLights.size(); lightIndex++ ) {
			if ( m_directionalLights[lightIndex] == _directionalLight ) {
				Debug::Get() << Debug::Severity::warning << "Directional Light already registered in gameobject : " << _directionalLight->GetGameobject()->GetName() << Debug::Endl();
				return;
			}
		}

		// Check num lights
		if ( m_directionalLights.size() >= GlobalValues::s_maximumNumDirectionalLight ) {
			Debug::Get() << Debug::Severity::warning << "Too much lights in the scene, maximum is " << GlobalValues::s_maximumNumDirectionalLight << Debug::Endl();
		} else {
			m_directionalLights.push_back( _directionalLight );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UnRegisterDirectionalLight	( DirectionalLight *	_directionalLight ) {

		const size_t num = m_directionalLights.size();

		// Removes the light
		for ( int lightIndex = 0; lightIndex < m_directionalLights.size(); lightIndex++ ) {
			if ( m_directionalLights[lightIndex] == _directionalLight ) {
				m_directionalLights.erase( m_directionalLights.begin() + lightIndex );
			}
		}

		// Light not removed
		if ( m_directionalLights.size() == num ) {
			Debug::Get() << Debug::Severity::warning << "Trying to remove a non registered directional light! gameobject=" << _directionalLight->GetGameobject()->GetName() << Debug::Endl();
			return;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::RegisterPointLight	( PointLight * _pointLight ) {

		// Looks for the _pointLight
		for ( int lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++ ) {
			if ( m_pointLights[lightIndex] == _pointLight ) {
				Debug::Get() << Debug::Severity::warning << "PointLight already registered in gameobject : " << _pointLight->GetGameobject()->GetName() << Debug::Endl();
				return;
			}
		}		

		// Check num lights
		if ( m_pointLights.size() >= GlobalValues::s_maximumNumPointLights ) {
			Debug::Get() << Debug::Severity::warning << "Too much lights in the scene, maximum is " << GlobalValues::s_maximumNumPointLights << Debug::Endl();
		} else {
			m_pointLights.push_back( _pointLight );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UnRegisterPointLight	( PointLight *	_pointLight ) {

		const size_t num = m_pointLights.size();

		// Removes the light
		for ( int lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++ ) {
			if ( m_pointLights[lightIndex] == _pointLight ) {
				m_pointLights.erase( m_pointLights.begin() + lightIndex );
			}
		}

		// Light not removed
		if ( m_pointLights.size() == num ) {
			Debug::Get() << Debug::Severity::warning << "Trying to remove a non registered point light! gameobject=" << _pointLight->GetGameobject()->GetName() << Debug::Endl();
			return;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::RegisterModel( Model * _model ) {
		// Looks for the model
		for ( int modelIndex = 0; modelIndex < m_models.size(); modelIndex++ ) {
			if ( m_models[modelIndex] == _model ) {
				Debug::Get() << Debug::Severity::warning << "Model already registered : " << _model->GetGameobject()->GetName() << Debug::Endl();
				return;
			}
		}
		m_models.push_back( _model );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UnRegisterModel( Model * _model ) {
		for ( int modelIndex = 0; modelIndex < m_models.size(); modelIndex++ ) {
			if ( m_models[modelIndex] == _model ) {
				m_models.erase( m_models.begin() + modelIndex );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnMaterialSetTexture( Material * _material, std::string _path ) {
		RessourceManager * texturesManager = m_renderer->GetRessourceManager();
		Texture * texture = texturesManager->FindTexture( _path );
		if ( texture == nullptr ) {
			texture = texturesManager->LoadTexture( _path );
		}
		_material->SetTexture( texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnModelSetPath( Model * _model, std::string _path ) {
		RessourceManager * ressourceManager = m_renderer->GetRessourceManager();
		Mesh * mesh = ressourceManager->FindMesh( _path );
		if ( mesh == nullptr ) {
			mesh = ressourceManager->LoadMesh( _path );
		}
		_model->SetMesh( mesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnGameobjectDeleted( Gameobject * _gameobject ) {
		if ( _gameobject == m_selectedGameobject ) {
			Deselect();
		}
	}
}