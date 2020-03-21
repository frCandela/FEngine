#include "fanEditor.hpp"

#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/fanRendererDebug.hpp"
#include "render/core/fanTexture.hpp"
#include "render/util/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
#include "render/fanRendererDebug.hpp"
#include "core/math/shapes/fanConvexHull.hpp"
#include "core/math/shapes/fanTriangle.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/math/shapes/fanPlane.hpp"
#include "core/math/shapes/fanAABB.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/input/fanJoystick.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanTime.hpp"
#include "editor/callbacks/fanGameWindowCallbacks.hpp"
#include "editor/windows/fanPreferencesWindow.hpp"	
#include "editor/windows/fanInspectorWindow.hpp"	
#include "editor/windows/fanProfilerWindow.hpp"	
#include "editor/windows/fanConsoleWindow.hpp"	
#include "editor/windows/fanNetworkWindow.hpp"	
#include "editor/windows/fanRenderWindow.hpp"	
#include "editor/windows/fanSceneWindow.hpp"	
#include "editor/windows/fanGameWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"	
#include "editor/fanEditorSelection.hpp"
#include "editor/fanEditorCopyPaste.hpp"
#include "editor/fanEditorGizmos.hpp"
#include "editor/fanEditorDebug.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletonComponents/fanEditorCamera.hpp"
#include "editor/singletonComponents/fanEditorGrid.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanTransformUI.hpp"
#include "scene/components/fanUIRenderer.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/systems/fanDrawDebug.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletonComponents/fanSceneInstantiate.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/fanPrefabManager.hpp"
#include "game/fanGame.hpp"
#include "game/components/fanPlanet.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		 m_game( "game", &Engine::InitializeGameEcsWorldTypes )
		,m_applicationShouldExit( false )
		,m_editorWorld( &Engine::InitializeEditorEcsWorldTypes )
	{
		// Get serialized editor values
		VkExtent2D windowSize = { 1280,720 };
		SerializedValues::Get().GetUInt( "renderer_extent_width", windowSize.width );
		SerializedValues::Get().GetUInt( "renderer_extent_height", windowSize.height );

		glm::ivec2 windowPosition = { 0,23 };
		SerializedValues::Get().GetInt( "renderer_position_x", windowPosition.x );
		SerializedValues::Get().GetInt( "renderer_position_y", windowPosition.y );

		SerializedValues::Get().LoadKeyBindings();

		// Creates keyboard events
		Input::Get().Manager().CreateKeyboardEvent( "delete", Keyboard::DELETE );
		Input::Get().Manager().CreateKeyboardEvent( "open_scene", Keyboard::O, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "save_scene", Keyboard::S, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "reload_scene", Keyboard::R, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "freeze_capture", Keyboard::END );
		Input::Get().Manager().CreateKeyboardEvent( "copy", Keyboard::C, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "paste", Keyboard::V, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "toogle_camera", Keyboard::TAB );
		Input::Get().Manager().CreateKeyboardEvent( "toogle_view", Keyboard::F1 );
		Input::Get().Manager().CreateKeyboardEvent( "show_ui", Keyboard::F3 );
		Input::Get().Manager().CreateKeyboardEvent( "play_pause", Keyboard::F5 );
		Input::Get().Manager().CreateKeyboardEvent( "reload_shaders", Keyboard::F11 );
		Input::Get().Manager().CreateKeyboardEvent( "reload_icons", Keyboard::F12 );

		//editor axis
		Input::Get().Manager().CreateKeyboardAxis( "editor_forward", Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateKeyboardAxis( "editor_left", Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateKeyboardAxis( "editor_up", Keyboard::E, Keyboard::Q );
		Input::Get().Manager().CreateKeyboardAxis( "editor_boost", Keyboard::LEFT_SHIFT, Keyboard::NONE );

		// game axis
		Input::Get().Manager().CreateKeyboardAxis( "game_forward", Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateKeyboardAxis( "game_left", Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateKeyboardAxis( "game_fire", Keyboard::SPACE, Keyboard::NONE );
		Input::Get().Manager().CreateKeyboardAxis( "game_boost", Keyboard::LEFT_SHIFT, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardAxis( "game_axis_stop", Keyboard::LEFT_CONTROL, Keyboard::NONE );

		Input::Get().Manager().CreateJoystickAxis( "gamejs_x_axis_direction", 0, Joystick::RIGHT_X );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_y_axis_direction", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_forward", 0, Joystick::LEFT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_left", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_boost", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_fire", 0, Joystick::RIGHT_TRIGGER );
		Input::Get().Manager().CreateJoystickButtons( "gamejs_axis_stop", 0, Joystick::A );

		// renderer
		m_window = new Window( "FEngine", windowSize, windowPosition );
		m_renderer = new Renderer( *m_window );	

		Color clearColor;
		if ( SerializedValues::Get().GetColor( "clear_color", clearColor ) )
		{
			m_renderer->SetClearColor( clearColor.ToGLM() );
		}

		Scene& scene = m_game.world.GetSingletonComponent<Scene>();

		// Initialize editor components		
		m_selection = new EditorSelection( scene );
		m_copyPaste = new EditorCopyPaste( *m_selection );
		m_gizmos = new EditorGizmos( scene );
		m_gameCallbacks = new EditorGameWindowCallbacks( m_game );
		m_renderWindow = new RenderWindow();
		m_sceneWindow = new SceneWindow( scene );
		m_inspectorWindow = new InspectorWindow();
		m_consoleWindow = new ConsoleWindow();
		m_ecsWindow = new EcsWindow( *scene.world );
		m_profilerWindow = new ProfilerWindow();
		m_gameWindow = new GameWindow( m_game );
		m_preferencesWindow = new PreferencesWindow();
		m_networkWindow = new NetworkWindow( scene );
		m_mainMenuBar = new MainMenuBar( scene , *m_selection );

		RendererDebug::Init( &m_renderer->GetRendererDebug() );
		EditorGizmos::Init( m_gizmos );
		Prefab::s_resourceManager.Init();

		EditorGrid& grid = m_editorWorld.GetSingletonComponent<EditorGrid>();


		m_selection->ConnectCallbacks( scene );
		m_renderWindow->SetRenderer( m_renderer );
		m_preferencesWindow->SetRenderer( m_renderer );
		m_mainMenuBar->SetGrid( &grid );

		m_mainMenuBar->SetWindows( { m_renderWindow , m_sceneWindow , m_inspectorWindow , m_consoleWindow, m_ecsWindow, m_profilerWindow, m_gameWindow, m_networkWindow, m_preferencesWindow } );
		m_sceneWindow->onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, m_selection );

		// Instance messages				
		m_mainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, m_renderer );
		m_mainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, m_renderer );
		m_mainMenuBar->onExit.Connect( &Engine::Exit, this );
		m_selection->onSceneNodeSelected.Connect( &SceneWindow::OnSceneNodeSelected, m_sceneWindow );
		m_selection->onSceneNodeSelected.Connect( &InspectorWindow::OnSceneNodeSelected, m_inspectorWindow );

		// Events linking
		m_gameCallbacks->ConnectCallbacks( *m_gameWindow, *m_renderer );
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect( &Engine::SwitchPlayStop, this );
		Input::Get().Manager().FindEvent( "copy" )->Connect( &EditorCopyPaste::OnCopy, m_copyPaste );
		Input::Get().Manager().FindEvent( "paste" )->Connect( &EditorCopyPaste::OnPaste, m_copyPaste );
		Input::Get().Manager().FindEvent( "show_ui" )->Connect( &Engine::OnToogleShowUI, this );
		//Input::Get().Manager().FindEvent( "toogle_view" )->Connect( &Engine::OnToogleView, this );
		Input::Get().Manager().FindEvent( "toogle_camera" )->Connect( &Engine::OnToogleCamera, this );

		scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		scene.onLoad.Connect( &Engine::OnSceneLoad, this );
		m_game.onStop.Connect( &Engine::OnGameStop, this );

		m_game.scene.New();

 		//scene->LoadFrom( "content/scenes/game.scene" );
// 		m_currentScene = _scene;
// 		m_sceneWindow->SetScene( m_currentScene );
// 		m_mainMenuBar->SetScene( m_currentScene );
// 		m_ecsWindow->SetEcsWorld( m_currentScene->GetWorld() );
// 		m_gameWindow->SetScene( m_currentScene );

		Debug::Log( "done initializing" );
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine()
	{
		// Deletes ui
		delete m_mainMenuBar;

		// Serialize editor positions
		const VkExtent2D rendererSize = m_window->GetExtent();
		const glm::ivec2 windowPosition = m_window->GetPosition();
		SerializedValues::Get().SetUInt( "renderer_extent_width", rendererSize.width );
		SerializedValues::Get().SetUInt( "renderer_extent_height", rendererSize.height );
		SerializedValues::Get().SetInt( "renderer_position_x", windowPosition.x );
		SerializedValues::Get().SetInt( "renderer_position_y", windowPosition.y );
		SerializedValues::Get().SaveValuesToDisk();

		Prefab::s_resourceManager.Clear();

		delete m_renderer;
		delete m_window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Exit()
	{
		m_applicationShouldExit = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{
		Clock logicClock;
		Clock renderClock;

		Profiler::Get().Begin();

		while ( m_applicationShouldExit == false && m_window->IsOpen() == true )
		{
			const float time = Time::ElapsedSinceStartup();

			// Runs logic, renders ui
			const float targetLogicDelta = Time::Get().GetLogicDelta();
			if ( logicClock.ElapsedSeconds() > targetLogicDelta )
			{
				logicClock.Reset();

				SCOPED_PROFILE( logic )
				{
					SCOPED_PROFILE( init )					
					Input::Get().NewFrame();
					Mouse::Get().Update( m_gameWindow->GetPosition(), m_gameWindow->GetSize(), m_gameWindow->IsHovered() );
					ImGui::NewFrame();					
					ImGui::GetIO().DeltaTime = targetLogicDelta;
					m_renderer->GetRendererDebug().ClearDebug();

					onLPPSynch.Emmit();
				}

				// update
				std::vector< Game* > games = { &m_game };
				for( Game* game : games )
				{
					game->Step( targetLogicDelta );
					EditorCamera& editorCamera = game->world.GetSingletonComponent<EditorCamera>();
					EditorCamera::Update( editorCamera, targetLogicDelta );
				}			

				// ui & debug
				if ( m_showUI )
				{
					{
						SCOPED_PROFILE( draw_ui );
						m_mainMenuBar->Draw();
						m_selection->Update( m_gameWindow->IsHovered() );
					}					

					{
						SCOPED_PROFILE( debug_draw );
						EditorGrid::Draw( m_editorWorld.GetSingletonComponent<EditorGrid>() );

						EcsWorld& world = m_game.world;
						if( m_mainMenuBar->ShowWireframe() )
						{
							const Signature signatureDrawDebugWireframe = S_DrawDebugWireframe::GetSignature( world );
							S_DrawDebugWireframe::Run( world, world.Match( signatureDrawDebugWireframe ) );
						}
						if( m_mainMenuBar->ShowNormals() )
						{
							const Signature signatureDrawDebugNormals = S_DrawDebugNormals::GetSignature( world );
							S_DrawDebugNormals::Run( world, world.Match( signatureDrawDebugNormals ) );
						}
						if( m_mainMenuBar->ShowAABB() )
						{
							const Signature signatureDrawDebugBounds = S_DrawDebugBounds::GetSignature( world );
							S_DrawDebugBounds::Run( world, world.Match( signatureDrawDebugBounds ) );
						}
						if( m_mainMenuBar->ShowHull() )
						{
							const Signature signatureDrawDebugHull = S_DrawDebugHull::GetSignature( world );
							S_DrawDebugHull::Run( world, world.Match( signatureDrawDebugHull ) );
						}
						if( m_mainMenuBar->ShowLights() )
						{
							const Signature signatureDrawDebugPointLights = S_DrawDebugPointLights::GetSignature( world );
							S_DrawDebugPointLights::Run( world, world.Match( signatureDrawDebugPointLights ) );
							const Signature signatureDrawDebugDirLights = S_DrawDebugDirectionalLights::GetSignature( world );
							S_DrawDebugDirectionalLights::Run( world, world.Match( signatureDrawDebugDirLights ) );
						}
					}					
				}

				Input::Get().Manager().PullEvents();

				{
					SCOPED_PROFILE( imgui_render )
					ImGui::Render();
				}
			}

			// Render world
			const float targetRenderDelta = Time::Get().GetRenderDelta();
			if ( renderClock.ElapsedSeconds() > targetRenderDelta )
			{
				renderClock.Reset();
				Time::Get().RegisterFrameDrawn();	// used for stats
				
				UpdateRenderWorld();

				m_renderer->DrawFrame();
				Profiler::Get().End();
				Profiler::Get().Begin();
			}
		}

		// Exit sequence
		Debug::Log( "Exit application" );
	}

	//================================================================================================================================
	// switch to editor camera
	//================================================================================================================================
	void Engine::OnGameStop( Game& _game )
	{
		//@hack
// 		FPSCamera* editorCam = _scene->FindComponentOfType<FPSCamera>();
// 		if ( editorCam != nullptr )
// 		{
// 			_scene->SetMainCamera( editorCam->GetCamera() );
// 		}
	}

	//================================================================================================================================
	// Todo save camera position depending on the scene
	//================================================================================================================================
	void Engine::OnSceneLoad( Scene& _scene )
	{
		m_selection->Deselect();

		EcsWorld& world = *_scene.world;
		Scene& scene = world.GetSingletonComponent< Scene >();

		// Editor Camera
		SceneNode& cameraNode = scene.CreateSceneNode( "editor_camera", scene.root );
		EntityID id = world.GetEntityID( cameraNode.handle );
		cameraNode.AddFlag( SceneNode::NOT_SAVED | SceneNode::NO_DELETE );

		Transform& transform = world.AddComponent< Transform >( id );
		Camera&    camera = world.AddComponent< Camera >( id );

		transform.SetPosition( btVector3( 0, 0, -2 ) );

		scene.mainCamera = &cameraNode ;

		// set editor camera singleton
		EditorCamera& editorCamera = world.GetSingletonComponent<EditorCamera>();
		editorCamera.node = &cameraNode;
		editorCamera.transform = &transform;
		editorCamera.camera = &camera;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UpdateRenderWorld()
	{
		EcsWorld& world = m_game.world;
		RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();		
		renderWorld.targetSize = glm::vec2( m_gameWindow->GetSize().x(), m_gameWindow->GetSize().y() );

		const Signature signatureURModels = S_UpdateRenderWorldModels::GetSignature( world );
		const Signature signatureURUI = S_UpdateRenderWorldUI::GetSignature( world );
		const Signature signatureURPointLights = S_UpdateRenderWorldPointLights::GetSignature( world );
		const Signature signatureURDirectionalLights = S_UpdateRenderWorldDirectionalLights::GetSignature( world );

		// update render data
		S_UpdateRenderWorldModels::Run( world, world.Match( signatureURModels ) );
		S_UpdateRenderWorldUI::Run( world, world.Match( signatureURUI ) );
		S_UpdateRenderWorldPointLights::Run( world, world.Match( signatureURPointLights ) );
		S_UpdateRenderWorldDirectionalLights::Run( world, world.Match( signatureURDirectionalLights ) );

		// particles mesh
		DrawMesh particlesDrawData;
		particlesDrawData.mesh = &renderWorld.particlesMesh;
		particlesDrawData.modelMatrix = glm::mat4( 1.f );
		particlesDrawData.normalMatrix = glm::mat4( 1.f );
		particlesDrawData.color = glm::vec4( 1.f, 1.f, 1.f, 1.f );
		particlesDrawData.shininess = 1;
		particlesDrawData.textureIndex = 1;
		renderWorld.drawData.push_back( particlesDrawData );

		m_renderer->SetDrawData( renderWorld.drawData );
		m_renderer->SetUIDrawData( renderWorld.uiDrawData );
		m_renderer->SetPointLights( renderWorld.pointLights );
		m_renderer->SetDirectionalLights( renderWorld.directionalLights );

		// Camera
		EditorCamera& editorCamera = world.GetSingletonComponent<EditorCamera>();
		editorCamera.camera->aspectRatio = m_gameWindow->GetAspectRatio();
		m_renderer->SetMainCamera( 
			editorCamera.camera->GetProjection(), 
			editorCamera.camera->GetView( *editorCamera.transform ),
			ToGLM( editorCamera.transform->GetPosition() )
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SwitchPlayStop()
	{
		if ( m_game.state == Game::STOPPED )
		{
			m_game.Play();
		}
		else
		{
			m_game.Stop();
		}
	}

	//================================================================================================================================
	// Toogle current scene main camera between editor and game
	//================================================================================================================================
	void Engine::OnToogleCamera()
	{
		//@hack
// 		FPSCamera* editorCameraCtrl = m_currentScene->FindComponentOfType<FPSCamera>();
// 		CameraController* gameCameraCtrl = m_currentScene->FindComponentOfType<CameraController>();
// 
// 		
// 		if ( editorCameraCtrl != nullptr && gameCameraCtrl != nullptr )
// 		{
// 			Camera* editorCamera = editorCameraCtrl->GetGameobject().GetComponent<Camera>();
// 			Camera* gameCamera = gameCameraCtrl->GetGameobject().GetComponent<Camera>();
// 			if ( &m_currentScene->GetMainCamera() == editorCamera )
// 			{
// 				m_currentScene->SetMainCamera( gameCamera );
// 			}
// 			else
// 			{
// 				m_currentScene->SetMainCamera( editorCamera );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::InitializeEditorEcsWorldTypes( EcsWorld& _world )
	{
		_world.AddSingletonComponentType<EditorGrid>();

		//_world.AddComponentType<SceneNode>();

		//_world.AddTagType<tag_boundsOutdated>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::InitializeGameEcsWorldTypes( EcsWorld& _world )
	{
		_world.AddSingletonComponentType<RenderWorld>();
		_world.AddSingletonComponentType<PhysicsWorld>();
		_world.AddSingletonComponentType<EditorCamera>();
		_world.AddSingletonComponentType<Scene>();

		_world.AddSingletonComponentType<SunLight>();

		_world.AddComponentType<SceneNode>();
		_world.AddComponentType<Transform>();
		_world.AddComponentType<DirectionalLight>();
		_world.AddComponentType<PointLight>();
		_world.AddComponentType<MeshRenderer>();
		_world.AddComponentType<Material>();
		_world.AddComponentType<Camera>();
		_world.AddComponentType<ParticleEmitter>();
		_world.AddComponentType<Particle>();
		_world.AddComponentType<Rigidbody>();
		_world.AddComponentType<MotionState>();
		_world.AddComponentType<BoxShape>();
		_world.AddComponentType<SphereShape>();
		_world.AddComponentType<UITransform>();
		_world.AddComponentType<UIRenderer>();
		_world.AddComponentType<Bounds>();

		_world.AddComponentType<Planet>();

		_world.AddTagType<tag_boundsOutdated>();
		_world.AddTagType<tag_editorOnly>();
	}
}