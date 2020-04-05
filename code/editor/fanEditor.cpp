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
#include "editor/windows/fanPreferencesWindow.hpp"	
#include "editor/windows/fanSingletonsWindow.hpp"
#include "editor/windows/fanInspectorWindow.hpp"	
#include "editor/windows/fanProfilerWindow.hpp"	
#include "editor/windows/fanConsoleWindow.hpp"	
#include "editor/windows/fanNetworkWindow.hpp"	
#include "editor/windows/fanRenderWindow.hpp"	
#include "editor/windows/fanSceneWindow.hpp"	
#include "editor/windows/fanGameViewWindow.hpp"
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
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/components/ui/fanProgressBar.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanExpirationTime.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanDrawDebug.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanScenePointers.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/fanPrefab.hpp"

#include "game/fanGame.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		 m_game( new Game("game"))
		,m_applicationShouldExit( false )
		,m_editorWorld()
	{
		Engine::InitializeEditorEcsWorldTypes( m_editorWorld);
		Engine::InitializeGameEcsWorldTypes( m_game->world );

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
		Input::Get().Manager().CreateKeyboardEvent( "toogle_follow_transform_lock", Keyboard::L );

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

		Scene& scene = m_game->world.GetSingletonComponent<Scene>();

		// Initialize editor components		
		m_selection = new EditorSelection( scene );
		m_copyPaste = new EditorCopyPaste( *m_selection );
		m_gizmos = new EditorGizmos( scene );
		m_renderWindow = new RenderWindow();
		m_sceneWindow = new SceneWindow( scene );
		m_inspectorWindow = new InspectorWindow();
		m_consoleWindow = new ConsoleWindow();
		m_ecsWindow = new EcsWindow( *scene.world );
		m_profilerWindow = new ProfilerWindow();
		m_gameViewWindow = new GameViewWindow( *m_game );
		m_preferencesWindow = new PreferencesWindow();
		m_networkWindow = new NetworkWindow( scene );
		m_singletonsWindow = new SingletonsWindow( m_game->world );
		m_mainMenuBar = new MainMenuBar( *m_game, *m_selection );
		m_mainMenuBar->SetGrid( &m_editorWorld.GetSingletonComponent<EditorGrid>() );
		m_mainMenuBar->SetWindows( {
			  m_renderWindow
			, m_sceneWindow
			, m_inspectorWindow
			, m_consoleWindow
			, m_ecsWindow
			, m_singletonsWindow
			, m_profilerWindow
			, m_gameViewWindow
			, m_networkWindow
			, m_preferencesWindow
			} );
		RendererDebug::Init( &m_renderer->GetRendererDebug() );
		EditorGizmos::Init( m_gizmos );
		Prefab::s_resourceManager.Init();
		m_selection->ConnectCallbacks( scene );
		m_renderWindow->SetRenderer( m_renderer );
		m_preferencesWindow->SetRenderer( m_renderer );


		m_sceneWindow->onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, m_selection );

		// Instance messages				
		m_mainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, m_renderer );
		m_mainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, m_renderer );
		m_mainMenuBar->onExit.Connect( &Engine::Exit, this );
		m_selection->onSceneNodeSelected.Connect( &SceneWindow::OnSceneNodeSelected, m_sceneWindow );
		m_selection->onSceneNodeSelected.Connect( &InspectorWindow::OnSceneNodeSelected, m_inspectorWindow );

		// Events linking
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect( &Engine::SwitchPlayStop, this );
		Input::Get().Manager().FindEvent( "copy" )->Connect( &EditorCopyPaste::OnCopy, m_copyPaste );
		Input::Get().Manager().FindEvent( "paste" )->Connect( &EditorCopyPaste::OnPaste, m_copyPaste );
		Input::Get().Manager().FindEvent( "show_ui" )->Connect( &Engine::OnToogleShowUI, this );
		Input::Get().Manager().FindEvent( "toogle_camera" )->Connect( &Engine::OnToogleCamera, this );

		m_gameViewWindow->onSizeChanged.Connect( &Renderer::ResizeGame, m_renderer );
		m_gameViewWindow->onPlay.Connect( &Engine::OnGamePlay, this );
		m_gameViewWindow->onPause.Connect( &Engine::OnGamePause, this );
		m_gameViewWindow->onResume.Connect( &Engine::OnGameResume, this );
		m_gameViewWindow->onStop.Connect( &Engine::OnGameStop, this );
		m_gameViewWindow->onStep.Connect( &Engine::OnGameStep, this );
		scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		scene.onLoad.Connect( &Engine::OnSceneLoad, this );

		scene.New();

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

		delete m_game;
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
					Mouse::Get().Update( m_gameViewWindow->GetPosition(), m_gameViewWindow->GetSize(), m_gameViewWindow->IsHovered() );
					ImGui::NewFrame();					
					ImGui::GetIO().DeltaTime = targetLogicDelta;
					m_renderer->GetRendererDebug().ClearDebug();

					onLPPSynch.Emmit();
				}

				// update
				std::vector< Game* > games = { m_game };
				for( Game* game : games )
				{
					game->Step( targetLogicDelta );
					EditorCamera& editorCamera = game->world.GetSingletonComponent<EditorCamera>();
					Scene& scene = game->world.GetSingletonComponent<Scene>();
					
					// only update the editor camera when we are using it
					if( scene.mainCamera == editorCamera.cameraNode )
					{
						EditorCamera::Update( editorCamera, targetLogicDelta );
					}					
				}			

				// ui & debug
				if ( m_showUI )
				{
					{
						SCOPED_PROFILE( draw_ui );
						m_mainMenuBar->Draw();
						m_selection->Update( m_gameViewWindow->IsHovered() );
						S_MoveFollowTransforms::Run( m_game->world, m_game->world.Match( S_MoveFollowTransforms::GetSignature( m_game->world ) ) );
						S_MoveFollowTransformsUI::Run( m_game->world, m_game->world.Match( S_MoveFollowTransformsUI::GetSignature( m_game->world ) ) );
					}					

					{
						SCOPED_PROFILE( debug_draw );
						EditorGrid::Draw( m_editorWorld.GetSingletonComponent<EditorGrid>() );

						EcsWorld& world = m_game->world;
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
	//================================================================================================================================
	void Engine::OnGamePlay()
	{
		Scene& scene = m_game->world.GetSingletonComponent<Scene>();

		if( scene.path.empty() )
		{
			Debug::Warning() << "please save the scene before playing" << Debug::Endl();
			return;
		}

		assert( m_game->state == Game::STOPPED );
		scene.Save();
		m_game->Start();

		UseGameCamera();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnGameStop()
	{
		UseEditorCamera();

		Scene& scene = m_game->world.GetSingletonComponent<Scene>();

		// Saves the camera position for restoring it later
		const EntityID oldCameraID = m_game->world.GetEntityID( scene.mainCamera->handle );
		const btTransform oldCameraTransform = m_game->world.GetComponent<Transform>( oldCameraID ).transform;

		// save old selection
		SceneNode* prevSelectionNode = m_selection->GetSelectedSceneNode();
		const uint32_t prevSelectionID = prevSelectionNode != nullptr ? prevSelectionNode->uniqueID : 0;

		m_game->Stop();
		scene.LoadFrom( scene.path ); // reload

		// restore camera transform
		const EntityID newCameraID = m_game->world.GetEntityID( scene.mainCamera->handle );
		m_game->world.GetComponent<Transform>( newCameraID ).transform = oldCameraTransform;

		// restore selection
		if( prevSelectionID != 0 )
		{
			auto it = scene.nodes.find( prevSelectionID );
			if( it != scene.nodes.end() )
			{
				m_selection->SetSelectedSceneNode( it->second );
			}
		}
	}

	void Engine::OnGamePause() { m_game->Pause(); }
	void Engine::OnGameResume() { m_game->Resume(); }

	//================================================================================================================================
	//================================================================================================================================
	void Engine::OnGameStep()
	{
		m_game->Resume();
		m_game->Step( Time::Get().GetLogicDelta() );
		m_game->Pause();
	}

	//================================================================================================================================
	// Todo save camera position depending on the scene
	//================================================================================================================================
	void Engine::OnSceneLoad( Scene& _scene )
	{
		m_selection->Deselect();
		EditorCamera::CreateEditorCamera( m_game->world );
	}

	//================================================================================================================================
	// Updates the render world singleton component
	//================================================================================================================================
	void Engine::UpdateRenderWorld()
	{
		EcsWorld& world = m_game->world;
		RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();		
		renderWorld.targetSize = glm::vec2( m_gameViewWindow->GetSize().x(), m_gameViewWindow->GetSize().y() );

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
		Scene& scene = world.GetSingletonComponent<Scene>();
		EntityID cameraID = world.GetEntityID( scene.mainCamera->handle );
		Camera& camera = world.GetComponent<Camera>( cameraID );
		camera.aspectRatio = m_gameViewWindow->GetAspectRatio();
		Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
		m_renderer->SetMainCamera( 
			camera.GetProjection(), 
			camera.GetView( cameraTransform ),
			ToGLM( cameraTransform.GetPosition() )
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::SwitchPlayStop()
	{
		if ( m_game->state == Game::STOPPED )
		{
			OnGamePlay();
		}
		else
		{
			OnGameStop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UseEditorCamera()
	{
		Scene& scene = m_game->world.GetSingletonComponent<Scene>();
		EditorCamera& editorCamera = m_game->world.GetSingletonComponent<EditorCamera>();
		scene.SetMainCamera( *editorCamera.cameraNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::UseGameCamera()
	{
		Scene& scene = m_game->world.GetSingletonComponent<Scene>();
		GameCamera& gameCamera = m_game->world.GetSingletonComponent<GameCamera>();
		scene.SetMainCamera( *gameCamera.cameraNode );
	}

	//================================================================================================================================
	// toogle the camera between editor and game
	//================================================================================================================================
	void Engine::OnToogleCamera()
	{		
		if( m_game->state == Game::STOPPED )
		{ 
			Debug::Warning() << "You cannot toogle camera outside of play mode" << Debug::Endl();
			return; 
		}

		Scene& scene = m_game->world.GetSingletonComponent<Scene>();
		GameCamera& gameCamera = m_game->world.GetSingletonComponent<GameCamera>();
		EditorCamera& editorCamera = m_game->world.GetSingletonComponent<EditorCamera>();

		if ( scene.mainCamera == editorCamera.cameraNode )
		{
			UseGameCamera();
		}
		else
		{
			UseEditorCamera();
		}
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
		_world.AddSingletonComponentType<EditorCamera>();
		_world.AddTagType<tag_editorOnly>();
	}
}