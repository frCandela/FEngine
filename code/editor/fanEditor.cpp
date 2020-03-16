#include "fanEditor.hpp"

#include "scene/ecs/systems/fanUpdateRenderWorld.hpp"

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
#include "core/imgui/fanImguiIcons.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/input/fanJoystick.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/imgui/fanModals.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanTime.hpp"
#include "editor/callbacks/fanGameWindowCallbacks.hpp"
#include "editor/windows/fanPreferencesWindow.hpp"	
#include "editor/windows/fanInspectorWindow.hpp"	
#include "editor/windows/fanProfilerWindow.hpp"	
#include "editor/windows/fanConsoleWindow.hpp"	
#include "editor/windows/fanNetworkWindow.hpp"
#include "editor/components/fanFPSCamera.hpp"		
#include "editor/windows/fanRenderWindow.hpp"	
#include "editor/windows/fanSceneWindow.hpp"	
#include "editor/windows/fanGameWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"	
#include "editor/fanEditorSelection.hpp"
#include "editor/fanEditorCopyPaste.hpp"
#include "editor/fanEditorGizmos.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/fanEditorDebug.hpp"
#include "scene/components/ui/fanUIMeshRenderer.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/actors/fanActor.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"
#include "scene/fanPrefabManager.hpp"
#include "game/components/fanCameraController.hpp"

#include "scene/ecs/systems/fanDrawDebug.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/components/fanDirectionalLight2.hpp"
#include "scene/ecs/components/fanpointLight2.hpp"
#include "scene/ecs/components/fanParticleEmitter.hpp"
#include "scene/ecs/components/fanParticle.hpp"
#include "scene/ecs/components/fanCamera2.hpp"
#include "scene/ecs/components/fanRigidbody2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanSphereShape2.hpp"
#include "scene/ecs/components/fanBoxShape2.hpp"
#include "scene/ecs/components/fanTransformUI.hpp"
#include "scene/ecs/components/fanUIRenderer.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanMaterial2.hpp"
#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"

#include "scene/ecs/singletonComponents/fanRenderWorld.hpp"
#include "editor/singletonComponents/fanEditorCamera.hpp"
#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"

#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	Signal<Camera*> Engine::onSetCamera;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit( false )
	{
		// Get serialized editor values
		VkExtent2D windowSize = { 1280,720 };
		SerializedValues::Get().GetUInt( "renderer_extent_width", windowSize.width );
		SerializedValues::Get().GetUInt( "renderer_extent_height", windowSize.height );

		glm::ivec2 windowPosition = { 0,23 };
		SerializedValues::Get().GetInt( "renderer_position_x", windowPosition.x );
		SerializedValues::Get().GetInt( "renderer_position_y", windowPosition.y );

		SerializedValues::Get().GetBool( "editor_grid_show", m_editorGrid.isVisible );
		SerializedValues::Get().GetFloat( "editor_grid_spacing", m_editorGrid.spacing );
		SerializedValues::Get().GetInt( "editor_grid_linesCount", m_editorGrid.linesCount );
		SerializedValues::Get().GetColor( "editor_grid_color", m_editorGrid.color );
		SerializedValues::Get().GetVec3( "editor_grid_offset", m_editorGrid.offset );

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

		// Scene
		m_clientScene = new Scene( "mainScene", &Engine::InitializeEcsWorldTypes );
		m_clientScene->SetServer( false );
		m_serverScene = new Scene( "serverScene", &Engine::InitializeEcsWorldTypes );
		m_serverScene->SetServer( true );

		// Initialize editor components		
		m_selection = new EditorSelection( m_currentScene );
		m_copyPaste = new EditorCopyPaste( *m_selection );
		m_gizmos = new EditorGizmos( m_currentScene );
		m_gameCallbacks = new EditorGameWindowCallbacks( *m_clientScene, *m_serverScene );
		m_renderWindow = new RenderWindow();
		m_sceneWindow = new SceneWindow();
		m_inspectorWindow = new InspectorWindow();
		m_consoleWindow = new ConsoleWindow();
		m_ecsWindow = new EcsWindow();
		m_profilerWindow = new ProfilerWindow();
		m_gameWindow = new GameWindow();
		m_preferencesWindow = new PreferencesWindow();
		m_networkWindow = new NetworkWindow( m_clientScene, m_serverScene );
		m_mainMenuBar = new MainMenuBar( *m_selection );

		RendererDebug::Init( &m_renderer->GetRendererDebug() );
		EditorGizmos::Init( m_gizmos );
		Prefab::s_resourceManager.Init();

		m_selection->ConnectCallbacks( *m_clientScene, *m_serverScene );
		m_renderWindow->SetRenderer( m_renderer );
		m_preferencesWindow->SetRenderer( m_renderer );
		m_mainMenuBar->SetGrid( &m_editorGrid );
		SetCurrentScene( m_clientScene );

		m_mainMenuBar->SetWindows( { m_renderWindow , m_sceneWindow , m_inspectorWindow , m_consoleWindow, m_ecsWindow, m_profilerWindow, m_gameWindow, m_networkWindow, m_preferencesWindow } );
		m_sceneWindow->onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, m_selection );

		// Instance messages				
		m_mainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, m_renderer );
		m_mainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, m_renderer );
		m_mainMenuBar->onExit.Connect( &Engine::Exit, this );
		m_mainMenuBar->onSetScene.Connect( &Engine::OnSetCurrentScene, this );
		m_selection->onSceneNodeSelected.Connect( &SceneWindow::OnSceneNodeSelected, m_sceneWindow );
		m_selection->onSceneNodeSelected.Connect( &InspectorWindow::OnSceneNodeSelected, m_inspectorWindow );

		// Events linking
		m_gameCallbacks->ConnectCallbacks( *m_gameWindow, *m_renderer );
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect( &Engine::SwitchPlayStop, this );
		Input::Get().Manager().FindEvent( "copy" )->Connect( &EditorCopyPaste::OnCopy, m_copyPaste );
		Input::Get().Manager().FindEvent( "paste" )->Connect( &EditorCopyPaste::OnPaste, m_copyPaste );
		Input::Get().Manager().FindEvent( "show_ui" )->Connect( &Engine::OnToogleShowUI, this );
		Input::Get().Manager().FindEvent( "toogle_view" )->Connect( &Engine::OnToogleView, this );
		Input::Get().Manager().FindEvent( "toogle_camera" )->Connect( &Engine::OnToogleCamera, this );

		m_clientScene->onSceneLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		m_clientScene->onSceneLoad.Connect( &Engine::OnSceneLoad, this );
		m_clientScene->onSceneStop.Connect( &Engine::OnSceneStop, this );
		m_clientScene->New();

		m_serverScene->onSceneLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		m_serverScene->onSceneLoad.Connect( &Engine::OnSceneLoad, this );
		m_serverScene->onSceneStop.Connect( &Engine::OnSceneStop, this );
		m_serverScene->New();

		// try open scenes
//  		m_clientScene->LoadFrom( "content/scenes/game.scene" );
//  		m_serverScene->LoadFrom( "content/scenes/game.scene" );

		Debug::Log( "done initializing" );
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine()
	{
		// Deletes ui
		delete m_mainMenuBar;
		delete m_clientScene;
		delete m_serverScene;

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

				// Update
				std::vector< Scene* > scenes = { m_clientScene , m_serverScene };
				for( Scene* scene : scenes )
				{
					EcsWorld& world = scene->GetWorld();
					scene->Update( targetLogicDelta );


					EditorCamera& editorCamera = world.GetSingletonComponent<EditorCamera>();
					EditorCamera::Update( editorCamera, targetLogicDelta );
				}			

				if ( m_showUI )
				{
					SCOPED_PROFILE( draw_ui )
					m_mainMenuBar->Draw();
					m_selection->Update( m_gameWindow->IsHovered() );
					DrawEditorGrid();
				}

				Input::Get().Manager().PullEvents();

				if ( m_showUI )
				{
					SCOPED_PROFILE( debug_draw )
					EcsWorld& world = m_currentScene->GetWorld();
					if ( m_mainMenuBar->ShowWireframe() ) { 
						const Signature signatureDrawDebugWireframe = S_DrawDebugWireframe::GetSignature( world );
						S_DrawDebugWireframe::Run( world, world.Match( signatureDrawDebugWireframe ) );
					}
					if ( m_mainMenuBar->ShowNormals() ) 
					{ 
						const Signature signatureDrawDebugNormals = S_DrawDebugNormals::GetSignature( world );
						S_DrawDebugNormals::Run( world, world.Match( signatureDrawDebugNormals ) );
					}
					if( m_mainMenuBar->ShowAABB() ) 
					{ 
						const Signature signatureDrawDebugBounds = S_DrawDebugBounds::GetSignature( world );
						S_DrawDebugBounds::Run( world, world.Match( signatureDrawDebugBounds ) );
					}
					if ( m_mainMenuBar->ShowHull() ) 
					{ 
						const Signature signatureDrawDebugHull = S_DrawDebugHull::GetSignature( world );
						S_DrawDebugHull::Run( world, world.Match( signatureDrawDebugHull ) );
					}
					if( m_mainMenuBar->ShowLights() )
					{
						const Signature signatureDrawDebugPointLights= S_DrawDebugPointLights::GetSignature( world );
						S_DrawDebugPointLights::Run( world, world.Match( signatureDrawDebugPointLights ) );
						const Signature signatureDrawDebugDirLights = S_DrawDebugDirectionalLights::GetSignature( world );
						S_DrawDebugDirectionalLights::Run( world, world.Match( signatureDrawDebugDirLights ) );
					}
				}

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
	//================================================================================================================================
	void Engine::SetCurrentScene( Scene* _scene )
	{
		assert( _scene != nullptr );
		m_currentScene = _scene;
		m_sceneWindow->SetScene( m_currentScene );
		m_mainMenuBar->SetScene( m_currentScene );
		m_ecsWindow->SetEcsWorld( m_currentScene->GetWorld() );
		m_gameWindow->SetScene( m_currentScene );
	}

	//================================================================================================================================
	// switch to editor camera
	//================================================================================================================================
	void Engine::OnSceneStop( Scene& _scene )
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

		EcsWorld& world = _scene.GetWorld();



		// Editor Camera
		SceneNode& cameraNode = _scene.InstanciateSceneNode( "editor_camera", &_scene.GetRootNode() );
		EntityID id = world.GetEntityID( cameraNode.entityHandle );
		cameraNode.AddFlag( SceneNode::NOT_SAVED | SceneNode::NO_DELETE );

		Transform2& transform = world.AddComponent< Transform2 >( id );
		Camera2&    camera = world.AddComponent< Camera2 >( id );

		transform.SetPosition( btVector3( 0, 0, -2 ) );

		_scene.SetMainCamera( cameraNode );

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
		EcsWorld& world = m_currentScene->GetWorld();
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
	void Engine::DrawEditorGrid() const
	{
		if ( m_editorGrid.isVisible == true )
		{
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for ( int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++ )
			{
				RendererDebug::Get().DebugLine( m_editorGrid.offset + btVector3( -count * size, 0.f, coord * size ), m_editorGrid.offset + btVector3( count * size, 0.f, coord * size ), m_editorGrid.color );
				RendererDebug::Get().DebugLine( m_editorGrid.offset + btVector3( coord * size, 0.f, -count * size ), m_editorGrid.offset + btVector3( coord * size, 0.f, count * size ), m_editorGrid.color );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SwitchPlayStop()
	{
		assert( m_clientScene->GetState() == m_serverScene->GetState() );
		if ( m_clientScene->GetState() == Scene::STOPPED )
		{
			m_clientScene->Play();
			m_serverScene->Play();
		}
		else
		{
			m_clientScene->Stop();
			m_serverScene->Stop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnSetCurrentScene( int _scene )
	{
		MainMenuBar::CurrentScene scene = MainMenuBar::CurrentScene( _scene );
		if ( scene == MainMenuBar::CLIENTS )
		{
			SetCurrentScene( m_clientScene );
		}
		else if ( scene == MainMenuBar::SERVER )
		{
			SetCurrentScene( m_serverScene );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnToogleView()
	{
		SetCurrentScene( m_currentScene == m_serverScene ? m_clientScene : m_serverScene );
	}

	//================================================================================================================================
	// Toogle current scene main camera between editor and game
	//================================================================================================================================
	void Engine::OnToogleCamera()
	{
		FPSCamera* editorCameraCtrl = m_currentScene->FindComponentOfType<FPSCamera>();
		CameraController* gameCameraCtrl = m_currentScene->FindComponentOfType<CameraController>();

		//@hack
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
	void Engine::InitializeEcsWorldTypes( EcsWorld& _world )
	{
		_world.AddSingletonComponentType<RenderWorld>();
		_world.AddSingletonComponentType<PhysicsWorld>();
		_world.AddSingletonComponentType<EditorCamera>();

		_world.AddComponentType<SceneNode>();
		_world.AddComponentType<Transform2>();
		_world.AddComponentType<DirectionalLight2>();
		_world.AddComponentType<PointLight2>();
		_world.AddComponentType<MeshRenderer2>();
		_world.AddComponentType<Material2>();
		_world.AddComponentType<Camera2>();
		_world.AddComponentType<ParticleEmitter>();
		_world.AddComponentType<Particle>();
		_world.AddComponentType<Rigidbody2>();
		_world.AddComponentType<MotionState>();
		_world.AddComponentType<BoxShape2>();
		_world.AddComponentType<SphereShape2>();
		_world.AddComponentType<UITransform2>();
		_world.AddComponentType<UIRenderer2>();
		_world.AddComponentType<Bounds>();

		_world.AddTagType<tag_boundsOutdated>();
		_world.AddTagType<tag_editorOnly>();
	}
}