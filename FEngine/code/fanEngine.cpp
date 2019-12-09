#include "fanGlobalIncludes.h"
#include "fanEngine.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanRendererDebug.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/util/fanWindow.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanUIMesh.h"
#include "core/time/fanTime.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/input/fanInputManager.h"
#include "core/input/fanJoystick.h"
#include "core/math/shapes/fanTriangle.h"
#include "core/math/shapes/fanPlane.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/fanBasicModels.h"
#include "core/files/fanFbxImporter.h"
#include "editor/fanModals.h"
#include "editor/fanMainMenuBar.h"
#include "editor/fanEditorDebug.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "editor/windows/fanConsoleWindow.h"	
#include "editor/windows/fanEcsWindow.h"	
#include "editor/windows/fanProfilerWindow.h"	
#include "editor/windows/fanGameWindow.h"
#include "editor/windows/fanNetworkWindow.h"
#include "editor/components/fanFPSCamera.h"		
#include "editor/callbacks/fanGameWindowCallbacks.h"
#include "editor/fanEditorSelection.h"
#include "editor/fanEditorGizmos.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanEditorCopyPaste.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/fanRessourcePtr.h"
#include "scene/fanRessourcePtr.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/actors/fanActor.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanDirectionalLight.h"
#include "scene/components/fanRigidbody.h"
#include "scene/components/fanSphereShape.h"
#include "scene/components/ui/fanUIMeshRenderer.h"
#include "scene/components/ui/fanUITransform.h"
#include "scene/fanSceneInstantiate.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/time/fanProfiler.h"

#include "scene/fanPrefab.h"


namespace fan 
{
	Signal<Camera*> Engine::onSetCamera;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit(false)
	{
		// Get serialized editor values
		VkExtent2D windowSize = { 1280,720 };
		SerializedValues::Get().GetUInt("renderer_extent_width", windowSize.width);
		SerializedValues::Get().GetUInt("renderer_extent_height", windowSize.height);

		glm::ivec2 windowPosition = { 0,23 };
		SerializedValues::Get().GetInt("renderer_position_x", windowPosition.x);
		SerializedValues::Get().GetInt("renderer_position_y", windowPosition.y);

		SerializedValues::Get().GetBool( "editor_grid_show", m_editorGrid.isVisible );
		SerializedValues::Get().GetFloat( "editor_grid_spacing", m_editorGrid.spacing );
		SerializedValues::Get().GetInt( "editor_grid_linesCount", m_editorGrid.linesCount );
		SerializedValues::Get().GetColor( "editor_grid_color", m_editorGrid.color );
		SerializedValues::Get().GetVec3( "editor_grid_offset", m_editorGrid.offset );

		SerializedValues::Get().LoadKeyBindings();

		// Creates keyboard events
		Input::Get().Manager().CreateKeyboardEvent( "delete",		  Keyboard::DELETE	);
		Input::Get().Manager().CreateKeyboardEvent( "reload_shaders", Keyboard::F5		);
		Input::Get().Manager().CreateKeyboardEvent( "reload_icons",	  Keyboard::F6		);
		Input::Get().Manager().CreateKeyboardEvent( "open_scene",	  Keyboard::O, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "save_scene",	  Keyboard::S, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "reload_scene",	  Keyboard::R, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent(	"play_pause",	  Keyboard::TAB );
		Input::Get().Manager().CreateKeyboardEvent( "freeze_capture", Keyboard::END );		
		Input::Get().Manager().CreateKeyboardEvent( "copy",			  Keyboard::C, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "paste",		  Keyboard::V, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "toogle_view",	  Keyboard::F1 );
		Input::Get().Manager().CreateKeyboardEvent( "show_ui",		  Keyboard::F3 );


		//editor axis
		Input::Get().Manager().CreateKeyboardAxis( "editor_forward", Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateKeyboardAxis( "editor_left",	 Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateKeyboardAxis( "editor_up",		 Keyboard::E, Keyboard::Q );
		Input::Get().Manager().CreateKeyboardAxis( "editor_boost",	 Keyboard::LEFT_SHIFT, Keyboard::NONE );

		// game axis
		Input::Get().Manager().CreateKeyboardAxis( "game_forward",	Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateKeyboardAxis( "game_left",		Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateKeyboardAxis( "game_fire",		Keyboard::SPACE, Keyboard::NONE );
		Input::Get().Manager().CreateKeyboardAxis( "game_boost",	Keyboard::LEFT_SHIFT, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardAxis( "game_axis_stop",Keyboard::LEFT_CONTROL, Keyboard::NONE );

		Input::Get().Manager().CreateJoystickAxis(		"gamejs_x_axis_direction"	, 0, Joystick::RIGHT_X );
		Input::Get().Manager().CreateJoystickAxis(		"gamejs_y_axis_direction"	, 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis(		"gamejs_forward"			, 0, Joystick::LEFT_Y );
		Input::Get().Manager().CreateJoystickAxis(		"gamejs_axis_left"			, 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis(		"gamejs_axis_boost"			, 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis(		"gamejs_axis_fire"			, 0, Joystick::RIGHT_TRIGGER );
		Input::Get().Manager().CreateJoystickButtons(	"gamejs_axis_stop"			, 0, Joystick::A );	

		// renderer
		m_renderer	= new Renderer( windowSize, windowPosition );
		Color clearColor;
		if ( SerializedValues::Get().GetColor( "clear_color", clearColor ) )
		{
			m_renderer->SetClearColor( clearColor.ToGLM() );
		}

		// Scene
		m_clientScene = new Scene( "mainScene" );
		m_serverScene = new Scene( "serverScene" );

		// Initialize editor components		
		m_selection			= new EditorSelection( m_currentScene);
		m_copyPaste			= new EditorCopyPaste(*m_selection);
		m_gizmos			= new EditorGizmos( m_currentScene);
		m_gameCallbacks		= new EditorGameWindowCallbacks( *m_clientScene, *m_serverScene );
		m_renderWindow		= new RenderWindow();
		m_sceneWindow		= new SceneWindow();
		m_inspectorWindow	= new InspectorWindow();		
		m_consoleWindow		= new ConsoleWindow();
		m_ecsWindow			= new EcsWindow();
		m_profilerWindow	= new ProfilerWindow( );
		m_gameWindow		= new GameWindow();
		m_preferencesWindow = new PreferencesWindow();		
		m_networkWindow		= new NetworkWindow( );
		m_mainMenuBar		= new MainMenuBar( *m_selection);

		EditorDebug::Get().SetDebug( & m_renderer->GetRendererDebug(), m_gizmos );

		m_renderWindow->SetRenderer( m_renderer );
		m_preferencesWindow->SetRenderer( m_renderer );
		m_mainMenuBar->SetGrid( &m_editorGrid );
		SetCurrentScene( m_clientScene );

		m_mainMenuBar->SetWindows( { m_renderWindow , m_sceneWindow , m_inspectorWindow , m_consoleWindow, m_ecsWindow, m_profilerWindow, m_gameWindow, m_networkWindow, m_preferencesWindow } );
		m_sceneWindow->onSelectGameobject.Connect( &EditorSelection::SetSelectedGameobject, m_selection );		
		Mouse::Get().Init( m_gameWindow );

		// Instance messages				
		m_mainMenuBar->onReloadShaders.		Connect(&Renderer::ReloadShaders, m_renderer );
		m_mainMenuBar->onReloadIcons.		Connect(&Renderer::ReloadIcons, m_renderer );
		m_mainMenuBar->onExit.				Connect( &Engine::Exit, this );
		m_mainMenuBar->onSetScene.			Connect(&Engine::OnSetCurrentScene, this );
		m_selection->onGameobjectSelected.	Connect( &SceneWindow::OnGameobjectSelected, m_sceneWindow );
		m_selection->onGameobjectSelected.	Connect( &InspectorWindow::OnGameobjectSelected, m_inspectorWindow );

		// Events linking
		m_gameCallbacks->ConnectCallbacks( *m_gameWindow, *m_renderer );
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect(	&Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect(		&Engine::SwitchPlayStop, this );
		Input::Get().Manager().FindEvent( "copy" )->Connect(			&EditorCopyPaste::OnCopy, m_copyPaste );
		Input::Get().Manager().FindEvent( "paste" )->Connect(			&EditorCopyPaste::OnPaste, m_copyPaste );
		Input::Get().Manager().FindEvent( "show_ui" )->Connect(			&Engine::OnToogleShowUI, this );
		Input::Get().Manager().FindEvent( "toogle_view" )->Connect(		&Engine::OnToogleView, this );

		// Static messages		
		TexturePtr::s_onCreateUnresolved.			Connect ( &Engine::OnResolveTexturePtr, this );
		MeshPtr::s_onCreateUnresolved.				Connect	( &Engine::OnResolveMeshPtr, this );
		PrefabPtr::s_onCreateUnresolved.			Connect ( &Engine::OnResolvePrefabPtr, this );

		m_clientScene->onSceneLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		m_clientScene->onSceneLoad.Connect( &Engine::OnSceneLoad, this );
		m_clientScene->onSceneStop.Connect( &Engine::OnSceneStop, this );

		m_clientScene->onRegisterMeshRenderer.Connect	( &Scene::RegisterMeshRenderer, m_clientScene );
		m_clientScene->onUnRegisterMeshRenderer.Connect	( &Scene::UnRegisterMeshRenderer, m_clientScene );
		m_clientScene->onPointLightAttach.Connect		( &Scene::RegisterPointLight, m_clientScene );
		m_clientScene->onPointLightDetach.Connect		( &Scene::UnRegisterPointLight, m_clientScene );
		m_clientScene->onDirectionalLightAttach.Connect	( &Scene::RegisterDirectionalLight, m_clientScene );
		m_clientScene->onDirectionalLightDetach.Connect	( &Scene::UnRegisterDirectionalLight, m_clientScene );
		m_clientScene->New();

		m_serverScene->onSceneLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		m_serverScene->onSceneLoad.Connect( &Engine::OnSceneLoad, this );
		m_serverScene->onSceneStop.Connect( &Engine::OnSceneStop, this );
		m_serverScene->onRegisterMeshRenderer.Connect	( &Scene::RegisterMeshRenderer, m_serverScene );
		m_serverScene->onUnRegisterMeshRenderer.Connect	( &Scene::UnRegisterMeshRenderer, m_serverScene );
		m_serverScene->onPointLightAttach.Connect		( &Scene::RegisterPointLight, m_serverScene );
		m_serverScene->onPointLightDetach.Connect		( &Scene::UnRegisterPointLight, m_serverScene );
		m_serverScene->onDirectionalLightAttach.Connect	( &Scene::RegisterDirectionalLight, m_serverScene );
		m_serverScene->onDirectionalLightDetach.Connect	( &Scene::UnRegisterDirectionalLight, m_serverScene );
		m_serverScene->New();
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		// Deletes ui
		delete m_mainMenuBar;
		delete m_clientScene;
		delete m_serverScene;

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
					m_renderer->GetRendererDebug().ClearDebug();
				}
				
				m_currentScene->Update( targetLogicDelta );		

				if ( m_showUI )				
				{
					SCOPED_PROFILE( draw_ui )					
					m_mainMenuBar->Draw();
					m_networkWindow->Update(targetLogicDelta);
					m_selection->Update( m_gameWindow->IsHovered() );
					DrawEditorGrid();
				}

				Input::Get().Manager().PullEvents();				

				if ( m_showUI )		
				{
					SCOPED_PROFILE( debug_draw )
					if ( m_mainMenuBar->ShowWireframe() ) { DrawWireframe(); }
					if ( m_mainMenuBar->ShowNormals() ) { DrawNormals(); }
					if ( m_mainMenuBar->ShowAABB() ) { DrawAABB(); }
					if ( m_mainMenuBar->ShowHull() ) { DrawHull(); }
				}			

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
	//================================================================================================================================
	void Engine::SetCurrentScene( Scene * _scene )
	{
		assert( _scene != nullptr );
		m_currentScene = _scene;
		m_sceneWindow->SetScene( m_currentScene );
		m_mainMenuBar->SetScene( m_currentScene );
		m_ecsWindow->SetEcsManager( m_currentScene->GetEcsManager() );
		m_networkWindow->SetScene(m_currentScene);
		m_gameWindow->SetScene(m_currentScene);
	}

	//================================================================================================================================
	// switch to editor camera
	//================================================================================================================================
	void Engine::OnSceneStop( Scene * _scene )
	{			
		FPSCamera * editorCam = _scene->FindComponentOfType<FPSCamera>();
		if ( editorCam != nullptr )
		{
			_scene->SetMainCamera( editorCam->GetCamera() );
		}
	}

	//================================================================================================================================
	// Todo save camera position depending on the scene
	//================================================================================================================================
	void Engine::OnSceneLoad(Scene * _scene) {

		m_selection->Deselect();

		// Editor Camera
		Gameobject * cameraGameobject = _scene->CreateGameobject("editor_camera");
		cameraGameobject->SetEditorFlags( 
			Gameobject::EditorFlag::NO_DELETE			| 
			Gameobject::EditorFlag::NOT_SAVED			|  
			Gameobject::EditorFlag::ALWAYS_PLAY_ACTORS
		);


		cameraGameobject->GetTransform()->SetPosition(btVector3(0, 0, -2));
		Camera* editorCamera = cameraGameobject->AddComponent<Camera>();
		editorCamera->SetRemovable(false);
		FPSCamera * editorCamController = cameraGameobject->AddComponent<FPSCamera>();
		editorCamController->SetRemovable(false);		

		_scene->SetMainCamera( editorCamera );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawEditorGrid() const {
		if (m_editorGrid.isVisible == true) {
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for (int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++) {
				EditorDebug::Get().Renderer().DebugLine( m_editorGrid.offset + btVector3(-count * size, 0.f, coord*size), m_editorGrid.offset + btVector3(count*size, 0.f, coord*size), m_editorGrid.color);
				EditorDebug::Get().Renderer().DebugLine( m_editorGrid.offset + btVector3(coord*size, 0.f, -count * size), m_editorGrid.offset + btVector3(coord*size, 0.f, count*size), m_editorGrid.color);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawAABB() const {
		const std::vector< Gameobject *>  & entities = m_currentScene->BuildEntitiesList();
		for (int gameobjectIndex = 0; gameobjectIndex < entities.size(); gameobjectIndex++) {
			const Gameobject * gameobject = entities[gameobjectIndex];
			if (gameobject != m_currentScene->GetMainCamera()->GetGameobject()) {
				AABB aabb = gameobject->GetAABB();
				EditorDebug::Get().Renderer().DebugAABB(aabb, Color::Red);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawHull() const	{
		Gameobject * selectedGameobject = m_selection->GetSelectedGameobject();
		if (selectedGameobject != nullptr) {
			MeshRenderer * meshRenderer = selectedGameobject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr) {
				const ConvexHull * hull = nullptr;
				Mesh * mesh = meshRenderer->GetMesh();
				if (mesh != nullptr) {
					hull = & meshRenderer->GetMesh()->GetHull();
				}
				if (hull != nullptr) {
					const std::vector<btVector3> & vertices = hull->GetVertices();
					const std::vector<uint32_t> & indices = hull->GetIndices();
					if (!vertices.empty()) {
						const glm::mat4  modelMat = meshRenderer->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();

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
							
							EditorDebug::Get().Renderer().DebugLine(worldVec0, worldVec1, color);
							EditorDebug::Get().Renderer().DebugLine(worldVec1, worldVec2, color);
							EditorDebug::Get().Renderer().DebugLine(worldVec2, worldVec0, color);
		
						}
					}
				}
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawWireframe() const {
		Gameobject * selectedGameobject = m_selection->GetSelectedGameobject();
		if (selectedGameobject != nullptr) {
			MeshRenderer * meshRenderer = selectedGameobject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr) {
				Mesh * mesh = meshRenderer->GetMesh();
				if (mesh != nullptr) {
					const glm::mat4  modelMat = meshRenderer->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
					const std::vector<uint32_t> & indices = mesh->GetIndices();
					const std::vector<Vertex> & vertices = mesh->GetVertices();

					for (int index = 0; index < indices.size() / 3; index++) {
						const btVector3 v0 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 0]].pos, 1.f));
						const btVector3 v1 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 1]].pos, 1.f));
						const btVector3 v2 = ToBullet(modelMat * glm::vec4(vertices[indices[3 * index + 2]].pos, 1.f));
						EditorDebug::Get().Renderer().DebugLine(v0, v1, Color::Yellow);
						EditorDebug::Get().Renderer().DebugLine(v1, v2, Color::Yellow);
						EditorDebug::Get().Renderer().DebugLine(v2, v0, Color::Yellow);
					}
				}
			}
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawNormals() const 
	{
		Gameobject * selectedGameobject = m_selection->GetSelectedGameobject();
		if ( selectedGameobject != nullptr) {
			MeshRenderer * meshRenderer = selectedGameobject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr) {
				Mesh * mesh = meshRenderer->GetMesh();
				if (mesh != nullptr) {
					const glm::mat4  modelMat = meshRenderer->GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
					const glm::mat4  normalMat = meshRenderer->GetGameobject()->GetComponent<Transform>()->GetNormalMatrix();
					const std::vector<uint32_t> & indices = mesh->GetIndices();
					const std::vector<Vertex> & vertices = mesh->GetVertices();

					for (int index = 0; index < indices.size(); index++) {
						const Vertex& vertex = vertices[indices[index]];
						const btVector3 position = ToBullet(modelMat * glm::vec4(vertex.pos, 1.f));
						const btVector3 normal = ToBullet(normalMat * glm::vec4(vertex.normal, 1.f));
						EditorDebug::Get().Renderer().DebugLine(position, position + 0.1f * normal, Color::Green);
					}
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SwitchPlayStop() {
		if ( m_currentScene->GetState() == Scene::STOPPED ) {
			m_currentScene->Play();
		} else {
			m_currentScene->Stop();
		}
	}

	//================================================================================================================================
	// TODO use an ecs system to do this
	//================================================================================================================================
	void Engine::UpdateRenderer() {
		SCOPED_PROFILE( update_renderer )

		Camera * mainCamera = m_currentScene->GetMainCamera();
		const std::vector < DirectionalLight* > directionalLights = m_currentScene->GetDirectionalLights();
		const std::vector < PointLight* >		pointLights = m_currentScene->GetPointLights();
		const std::vector < MeshRenderer* >		meshRenderers = m_currentScene->GetMeshRenderers();

		// Camera
		Transform * cameraTransform = mainCamera->GetGameobject()->GetComponent<Transform>();
		mainCamera->SetAspectRatio( m_gameWindow->GetAspectRatio() );
		m_renderer->SetMainCamera( mainCamera->GetProjection(), mainCamera->GetView(), ToGLM(cameraTransform->GetPosition()));		

		// Point lights		
		for ( int lightIndex = 0; lightIndex < pointLights.size(); lightIndex++ ) {
			const PointLight * light = pointLights[lightIndex];
			const Transform *  lightTransform = light->GetGameobject()->GetComponent<Transform>();
			m_renderer->SetPointLight(
				lightIndex,
				ToGLM( lightTransform->GetPosition() ),
				light->GetDiffuse().ToGLM(),
				light->GetSpecular().ToGLM(),
				light->GetAmbiant().ToGLM(),
				light->GetAttenuation()
			);
		}	m_renderer->SetNumPointLights( static_cast<uint32_t>( pointLights.size() ) );
		
		// Directional lights		
		for ( int lightIndex = 0; lightIndex < directionalLights.size(); lightIndex++ ) {
			const DirectionalLight * light			= directionalLights[lightIndex];
			const Transform *		 lightTransform = light->GetGameobject()->GetComponent<Transform>();
			m_renderer->SetDirectionalLight( 
				lightIndex
				,glm::vec4( ToGLM( lightTransform->Forward() ), 1 )
				,light->GetAmbiant().ToGLM()
				,light->GetDiffuse().ToGLM()
				,light->GetSpecular().ToGLM()
			);
		} m_renderer->SetNumDirectionalLights( static_cast<uint32_t>( directionalLights.size() ) );

		// Transforms, mesh, materials
		std::vector<DrawMesh> drawData( meshRenderers.size() );
		for (int modelIndex = 0; modelIndex < meshRenderers.size() ; modelIndex++) {

			DrawMesh& data = drawData[modelIndex];
			MeshRenderer * meshRenderer = meshRenderers[modelIndex];
			Transform * transform = meshRenderer->GetGameobject()->GetTransform();
			Material * material = meshRenderer->GetGameobject()->GetComponent<Material>();

			// Mesh
			data.mesh = meshRenderer->GetMesh();

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

		// UI
		std::vector< UIMeshRenderer* > uiRenderers = m_clientScene->FindComponentsOfType<UIMeshRenderer>();
		std::vector<DrawUIMesh> uiDrawData;
		uiDrawData.reserve( uiRenderers.size() );
		for (int meshIndex = 0; meshIndex < uiRenderers.size() ; meshIndex++)
		{
			UIMeshRenderer* meshRenderer = uiRenderers[meshIndex];
			if ( meshRenderer->IsVisible() && meshRenderer->GetMesh()->GetVertices().size() > 0 )
			{
				Transform * transform = meshRenderer->GetGameobject()->GetTransform();
				Texture * texture= meshRenderer->GetTexture();

				DrawUIMesh uiMesh;
				uiMesh.mesh = meshRenderer->GetMesh();
				uiMesh.scale = { transform->GetScale().x(), transform->GetScale().y() };				
				uiMesh.position = {transform->GetPosition().x(), transform->GetPosition().y()};
				uiMesh.color = meshRenderer->GetColor().ToGLM();
				uiMesh.textureIndex = texture != nullptr ? texture->GetRenderID() : 0 ;
				uiDrawData.push_back(uiMesh);
			}
		}
		m_renderer->SetUIDrawData(uiDrawData);

	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnResolveTexturePtr( TexturePtr * _ptr )
	{
		Texture * texture = RessourceManager::Get().FindTexture( _ptr->GetID() );
		if ( texture == nullptr )
		{
			texture = RessourceManager::Get().LoadTexture( _ptr->GetID() );
		}
		if( texture ) {
			*_ptr = TexturePtr( texture, texture->GetPath() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnResolveMeshPtr( MeshPtr * _ptr )
	{
		Mesh * mesh = RessourceManager::Get().FindMesh( _ptr->GetID() );
		if ( mesh == nullptr )
		{
			mesh = RessourceManager::Get().LoadMesh( _ptr->GetID() );
		}
		if( mesh ) {
			*_ptr = MeshPtr( mesh, mesh->GetPath() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Engine::OnResolvePrefabPtr( PrefabPtr * _ptr )
	{
		Prefab * prefab = RessourceManager::Get().FindPrefab( _ptr->GetID() );
		if ( prefab == nullptr )
		{
			prefab = RessourceManager::Get().LoadPrefab( _ptr->GetID() );
		}
		if ( prefab )
		{
			*_ptr = PrefabPtr( prefab, prefab->GetPath() );
		}
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnSetCurrentScene( int _scene )
	{
		MainMenuBar::CurrentScene scene = MainMenuBar::CurrentScene(_scene);
		if ( scene == MainMenuBar::CLIENTS )
		{
			SetCurrentScene(m_clientScene);
		}
		else if ( scene == MainMenuBar::SERVER )
		{
			SetCurrentScene(m_serverScene);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnToogleView()
	{
		SetCurrentScene( m_currentScene == m_serverScene ? m_clientScene : m_serverScene );
	}
}