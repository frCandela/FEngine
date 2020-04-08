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

#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Editor::Editor( const LaunchSettings _settings, EcsWorld& _gameWorld ) :
		m_gameWorld( _gameWorld )
		, m_applicationShouldExit( false )
		, m_launchSettings( _settings )
	{
		Editor::InitializeEditorEcsWorldTypes( m_editorWorld);
		Editor::InitializeGameEcsWorldTypes( m_gameWorld );

		// window position
		glm::ivec2 windowPosition = { 0,23 };
		if( _settings.window_position != glm::ivec2 (-1, -1) ) 
		{ 
			windowPosition = _settings.window_position;	
		}
		else
		{
			SerializedValues::Get().GetInt( "renderer_position_x", windowPosition.x );
			SerializedValues::Get().GetInt( "renderer_position_y", windowPosition.y );
		}

		// window size
		VkExtent2D windowSize = { 1280,720 };
		if( _settings.window_size != glm::ivec2( -1, -1 ) ) 
		{ 
			windowSize = { (uint32_t)_settings.window_size.x, (uint32_t)_settings.window_size.y };
		}
		else
		{
			SerializedValues::Get().GetUInt( "renderer_extent_width", windowSize.width );
			SerializedValues::Get().GetUInt( "renderer_extent_height", windowSize.height );
		}

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

		Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();

		// Initialize editor components		
		m_selection = new EditorSelection( scene );
		m_copyPaste = new EditorCopyPaste( *m_selection );
		m_gizmos = new EditorGizmos( m_gameWorld );
		m_renderWindow = new RenderWindow();
		m_sceneWindow = new SceneWindow( scene );
		m_inspectorWindow = new InspectorWindow();
		m_consoleWindow = new ConsoleWindow();
		m_ecsWindow = new EcsWindow( *scene.world );
		m_profilerWindow = new ProfilerWindow();
		m_gameViewWindow = new GameViewWindow( m_gameWorld );
		m_preferencesWindow = new PreferencesWindow();
		m_networkWindow = new NetworkWindow( scene );
		m_singletonsWindow = new SingletonsWindow( m_gameWorld );
		m_mainMenuBar = new MainMenuBar( m_gameWorld, *m_selection );
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
		m_mainMenuBar->onExit.Connect( &Editor::Exit, this );
		m_selection->onSceneNodeSelected.Connect( &SceneWindow::OnSceneNodeSelected, m_sceneWindow );
		m_selection->onSceneNodeSelected.Connect( &InspectorWindow::OnSceneNodeSelected, m_inspectorWindow );

		// Events linking
		Input::Get().Manager().FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
		Input::Get().Manager().FindEvent( "play_pause" )->Connect( &Editor::SwitchPlayStop, this );
		Input::Get().Manager().FindEvent( "copy" )->Connect( &EditorCopyPaste::OnCopy, m_copyPaste );
		Input::Get().Manager().FindEvent( "paste" )->Connect( &EditorCopyPaste::OnPaste, m_copyPaste );
		Input::Get().Manager().FindEvent( "show_ui" )->Connect( &Editor::OnToogleShowUI, this );
		Input::Get().Manager().FindEvent( "toogle_camera" )->Connect( &Editor::OnToogleCamera, this );

		m_gameViewWindow->onSizeChanged.Connect( &Renderer::ResizeGame, m_renderer );
		m_gameViewWindow->onPlay.Connect( &Editor::GameStart, this );
		m_gameViewWindow->onPause.Connect( &Editor::GamePause, this );
		m_gameViewWindow->onResume.Connect( &Editor::GameResume, this );
		m_gameViewWindow->onStop.Connect( &Editor::GameStop, this );
		m_gameViewWindow->onStep.Connect( &Editor::OnEditorStep, this );
		scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
		scene.onLoad.Connect( &Editor::OnSceneLoad, this );

		// load scene
		scene.New();
		if( ! _settings.loadScene.empty() )	
		{ 
			scene.LoadFrom( _settings.loadScene ); 

			// auto play the scene
			if( _settings.autoPlay )
			{
				GameStart();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Editor::~Editor()
	{
		// Deletes ui
		delete m_mainMenuBar;

		// Serialize editor positions if it was not modified by a launch command
		if( m_launchSettings.window_size == glm::ivec2( -1, -1 ) )
		{
			const VkExtent2D rendererSize = m_window->GetExtent();
			const glm::ivec2 windowPosition = m_window->GetPosition();
			SerializedValues::Get().SetUInt( "renderer_extent_width", rendererSize.width );
			SerializedValues::Get().SetUInt( "renderer_extent_height", rendererSize.height );
			SerializedValues::Get().SetInt( "renderer_position_x", windowPosition.x );
			SerializedValues::Get().SetInt( "renderer_position_y", windowPosition.y );
		}

		SerializedValues::Get().SaveValuesToDisk();

		Prefab::s_resourceManager.Clear();
		delete m_renderer;
		delete m_window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::Exit()
	{
		m_applicationShouldExit = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::Run()
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
				{
					GameStep( targetLogicDelta );

					EditorCamera& editorCamera = m_gameWorld.GetSingletonComponent<EditorCamera>();
					Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();

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
						S_MoveFollowTransforms::Run( m_gameWorld, m_gameWorld.Match( S_MoveFollowTransforms::GetSignature( m_gameWorld ) ) );
						S_MoveFollowTransformsUI::Run( m_gameWorld, m_gameWorld.Match( S_MoveFollowTransformsUI::GetSignature( m_gameWorld ) ) );
					}					

					{
						SCOPED_PROFILE( debug_draw );
						EditorGrid::Draw( m_editorWorld.GetSingletonComponent<EditorGrid>() );

						if( m_mainMenuBar->ShowWireframe() )
						{
							const Signature signatureDrawDebugWireframe = S_DrawDebugWireframe::GetSignature( m_gameWorld );
							S_DrawDebugWireframe::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugWireframe ) );
						}
						if( m_mainMenuBar->ShowNormals() )
						{
							const Signature signatureDrawDebugNormals = S_DrawDebugNormals::GetSignature( m_gameWorld );
							S_DrawDebugNormals::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugNormals ) );
						}
						if( m_mainMenuBar->ShowAABB() )
						{
							const Signature signatureDrawDebugBounds = S_DrawDebugBounds::GetSignature( m_gameWorld );
							S_DrawDebugBounds::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugBounds ) );
						}
						if( m_mainMenuBar->ShowHull() )
						{
							const Signature signatureDrawDebugHull = S_DrawDebugHull::GetSignature( m_gameWorld );
							S_DrawDebugHull::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugHull ) );
						}
						if( m_mainMenuBar->ShowLights() )
						{
							const Signature signatureDrawDebugPointLights = S_DrawDebugPointLights::GetSignature( m_gameWorld );
							S_DrawDebugPointLights::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugPointLights ) );
							const Signature signatureDrawDebugDirLights = S_DrawDebugDirectionalLights::GetSignature( m_gameWorld );
							S_DrawDebugDirectionalLights::Run( m_gameWorld, m_gameWorld.Match( signatureDrawDebugDirLights ) );
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
	void  Editor::GameStart()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::STOPPED )
		{
			// saves the scene before playing
			Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();
			if( scene.path.empty() )
			{
				Debug::Warning() << "please save the scene before playing" << Debug::Endl();
				return;
			}

			scene.Save();

			Debug::Highlight() << game.name << ": play" << Debug::Endl();
			game.state = Game::PLAYING;

			if( game.gameServer != nullptr ) game.gameServer->Start();
			else							 game.gameClient->Start();

			UseGameCamera();
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameStop()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::PLAYING || game.state == Game::PAUSED )
		{
			UseEditorCamera();

			Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();

			// Saves the camera position for restoring it later
			const EntityID oldCameraID = m_gameWorld.GetEntityID( scene.mainCamera->handle );
			const btTransform oldCameraTransform = m_gameWorld.GetComponent<Transform>( oldCameraID ).transform;

			// save old selection
			SceneNode* prevSelectionNode = m_selection->GetSelectedSceneNode();
			const uint32_t prevSelectionID = prevSelectionNode != nullptr ? prevSelectionNode->uniqueID : 0;

			Debug::Highlight() << game.name << ": stopped" << Debug::Endl();
			game.state = Game::STOPPED;
			if( game.gameServer != nullptr ) game.gameServer->Stop();
			else							 game.gameClient->Stop();
			scene.LoadFrom( scene.path ); // reload

			// restore camera transform
			const EntityID newCameraID = m_gameWorld.GetEntityID( scene.mainCamera->handle );
			m_gameWorld.GetComponent<Transform>( newCameraID ).transform = oldCameraTransform;

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
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GamePause()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::PLAYING )
		{
			Debug::Highlight() << game.name << ": paused" << Debug::Endl();
			game.state = Game::PAUSED;

			if( game.gameServer != nullptr ) game.gameServer->Pause();
			else							 game.gameClient->Pause();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameResume()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::PAUSED )
		{
			Debug::Highlight() << game.name << ": resumed" << Debug::Endl();
			game.state = Game::PLAYING;

			if( game.gameServer != nullptr ) game.gameServer->Resume();
			else							 game.gameClient->Resume();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameStep( const float _delta )
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::PLAYING )
		{
			const float delta = ( game.state == Game::PLAYING ? _delta : 0.f );
			if( game.gameServer != nullptr ) game.gameServer->Step( delta );
			else							 game.gameClient->Step( delta );
		}
	}

	//================================================================================================================================
	// Todo save camera position depending on the scene
	//================================================================================================================================
	void Editor::OnSceneLoad( Scene& _scene )
	{
		m_selection->Deselect();
		EditorCamera::CreateEditorCamera( m_gameWorld );
	}

	//================================================================================================================================
	// Updates the render world singleton component
	//================================================================================================================================
	void Editor::UpdateRenderWorld()
	{
		RenderWorld& renderWorld = m_gameWorld.GetSingletonComponent<RenderWorld>();
		renderWorld.targetSize = glm::vec2( m_gameViewWindow->GetSize().x(), m_gameViewWindow->GetSize().y() );

		const Signature signatureURModels = S_UpdateRenderWorldModels::GetSignature( m_gameWorld );
		const Signature signatureURUI = S_UpdateRenderWorldUI::GetSignature( m_gameWorld );
		const Signature signatureURPointLights = S_UpdateRenderWorldPointLights::GetSignature( m_gameWorld );
		const Signature signatureURDirectionalLights = S_UpdateRenderWorldDirectionalLights::GetSignature( m_gameWorld );

		// update render data
		S_UpdateRenderWorldModels::Run( m_gameWorld, m_gameWorld.Match( signatureURModels ) );
		S_UpdateRenderWorldUI::Run( m_gameWorld, m_gameWorld.Match( signatureURUI ) );
		S_UpdateRenderWorldPointLights::Run( m_gameWorld, m_gameWorld.Match( signatureURPointLights ) );
		S_UpdateRenderWorldDirectionalLights::Run( m_gameWorld, m_gameWorld.Match( signatureURDirectionalLights ) );

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
		Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();
		EntityID cameraID = m_gameWorld.GetEntityID( scene.mainCamera->handle );
		Camera& camera = m_gameWorld.GetComponent<Camera>( cameraID );
		camera.aspectRatio = m_gameViewWindow->GetAspectRatio();
		Transform& cameraTransform = m_gameWorld.GetComponent<Transform>( cameraID );
		m_renderer->SetMainCamera( 
			camera.GetProjection(), 
			camera.GetView( cameraTransform ),
			ToGLM( cameraTransform.GetPosition() )
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::SwitchPlayStop()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if ( game.state == Game::STOPPED )
		{
			GameStart();
		}
		else
		{
			GameStop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::UseEditorCamera()
	{
		Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();
		EditorCamera& editorCamera = m_gameWorld.GetSingletonComponent<EditorCamera>();
		scene.SetMainCamera( *editorCamera.cameraNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::UseGameCamera()
	{
		Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();
		GameCamera& gameCamera = m_gameWorld.GetSingletonComponent<GameCamera>();
		scene.SetMainCamera( *gameCamera.cameraNode );
	}

	//================================================================================================================================
	// toogle the camera between editor and game
	//================================================================================================================================
	void Editor::OnToogleCamera()
	{		
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		if( game.state == Game::STOPPED )
		{ 
			Debug::Warning() << "You cannot toogle camera outside of play mode" << Debug::Endl();
			return; 
		}

		Scene& scene = m_gameWorld.GetSingletonComponent<Scene>();
		GameCamera& gameCamera = m_gameWorld.GetSingletonComponent<GameCamera>();
		EditorCamera& editorCamera = m_gameWorld.GetSingletonComponent<EditorCamera>();

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
	// callback of the game view step button
	//================================================================================================================================
	void Editor::OnEditorStep()
	{
		Game& game = m_gameWorld.GetSingletonComponent<Game>();
		assert( game.state == Game::PAUSED );

		if( game.gameServer != nullptr )
		{
			game.gameServer->Resume();
			game.gameServer->Step( Time::Get().GetLogicDelta() );
			game.gameServer->Pause();
		}
		else
		{
			game.gameClient->Resume();
			game.gameClient->Step( Time::Get().GetLogicDelta() );
			game.gameClient->Pause();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::InitializeEditorEcsWorldTypes( EcsWorld& _world )
	{
		_world.AddSingletonComponentType<EditorGrid>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::InitializeGameEcsWorldTypes( EcsWorld& _world )
	{
		_world.AddSingletonComponentType<EditorCamera>();

		_world.AddTagType<tag_editorOnly>();
	}
}