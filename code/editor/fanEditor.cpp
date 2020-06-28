#include "fanEditor.hpp"

#include <thread>
#include "core/math/fanMathUtils.hpp"
#include "core/math/shapes/fanConvexHull.hpp"
#include "core/math/shapes/fanTriangle.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/math/shapes/fanPlane.hpp"
#include "core/math/shapes/fanAABB.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/input/fanJoystick.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/fanRendererDebug.hpp"
#include "render/core/fanTexture.hpp"
#include "render/util/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
#include "render/fanRendererDebug.hpp"
#include "network/singletons/fanTime.hpp"
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
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/fanEditorDebug.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
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
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/fanPrefab.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanGame.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Editor::Editor( const LaunchSettings _settings, std::vector<EcsWorld*> _gameWorlds ) :
		m_worlds( _gameWorlds )
		, m_applicationShouldExit( false )
		, m_launchSettings( _settings )
	{
		// Loop over all worlds to initialize them
		for( EcsWorld* worldPtr : m_worlds )
		{
			assert( worldPtr != nullptr );
			EcsWorld& world = *worldPtr;

			world.AddSingletonType<EditorCamera>();
			world.AddSingletonType<EditorGrid>();
			world.AddSingletonType<EditorSelection>();
			world.AddSingletonType<EditorCopyPaste>();
			world.AddSingletonType<EditorGizmos>();
			world.AddTagType<tag_editorOnly>();

			// window position
			glm::ivec2 windowPosition = { 0,23 };
			if( _settings.window_position != glm::ivec2( -1, -1 ) )
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

			Input::Get().Manager().CreateKeyboardEvent( "test", Keyboard::T );

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
			m_window = new Window( _settings.windowName.c_str(), windowSize, windowPosition );
			m_renderer = new Renderer( *m_window );

			Color clearColor;
			if( SerializedValues::Get().GetColor( "clear_color", clearColor ) )
			{
				m_renderer->SetClearColor( clearColor.ToGLM() );
			}

			Scene& scene = world.GetSingleton<Scene>();
			EditorSelection& selection = world.GetSingleton<EditorSelection>();
			EditorCopyPaste& copyPaste = world.GetSingleton<EditorCopyPaste>();

			// Initialize editor components		
			m_renderWindow = new RenderWindow();
			m_sceneWindow = new SceneWindow( scene );
			m_inspectorWindow = new InspectorWindow( world );
			m_consoleWindow = new ConsoleWindow();
			m_ecsWindow = new EcsWindow( world );
			m_profilerWindow = new ProfilerWindow();
			m_gameViewWindow = new GameViewWindow( world );
			m_preferencesWindow = new PreferencesWindow();
			m_networkWindow = new NetworkWindow( world );
			m_singletonsWindow = new SingletonsWindow( world );
			m_mainMenuBar = new MainMenuBar( world );
			m_mainMenuBar->SetGrid( &world.GetSingleton<EditorGrid>() );
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
			Prefab::s_resourceManager.Init();
			selection.ConnectCallbacks( scene );
			m_renderWindow->SetRenderer( m_renderer );
			m_preferencesWindow->SetRenderer( m_renderer );

			m_sceneWindow->onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );

			// Instance messages				
			m_mainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, m_renderer );
			m_mainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, m_renderer );
			m_mainMenuBar->onExit.Connect( &Editor::Exit, this );

			// Events linking
			Input::Get().Manager().FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
			Input::Get().Manager().FindEvent( "play_pause" )->Connect( &Editor::OnCurrentGameSwitchPlayStop, this );
			Input::Get().Manager().FindEvent( "copy" )->Connect( &EditorCopyPaste::OnCopy, &copyPaste );
			Input::Get().Manager().FindEvent( "paste" )->Connect( &EditorCopyPaste::OnPaste, &copyPaste );
			Input::Get().Manager().FindEvent( "show_ui" )->Connect( &Editor::OnToogleShowUI, this );
			Input::Get().Manager().FindEvent( "toogle_camera" )->Connect( &Editor::OnCurrentGameToogleCamera, this );

			m_gameViewWindow->onSizeChanged.Connect( &Renderer::ResizeGame, m_renderer );
			m_gameViewWindow->onPlay.Connect( &Editor::OnCurrentGameStart, this );
			m_gameViewWindow->onPause.Connect( &Editor::OnCurrentGamePause, this );
			m_gameViewWindow->onResume.Connect( &Editor::OnCurrentGameResume, this );
			m_gameViewWindow->onStop.Connect( &Editor::OnCurrentGameStop, this );
			m_gameViewWindow->onStep.Connect( &Editor::OnCurrentGameStep, this );
			scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, m_sceneWindow );
			scene.onLoad.Connect( &Editor::OnSceneLoad, this );

			// load scene
			scene.New();
			if( !_settings.loadScene.empty() )
			{
				scene.LoadFrom( _settings.loadScene );

				// auto play the scene
				if( _settings.autoPlay )
				{
					GameStart( world );
				}
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
		// initializes timers
		double lastRenderTime = Time::ElapsedSinceStartup();
		for( EcsWorld* world : m_worlds )
		{
			Time& time = world->GetSingleton<Time>();
			time.lastLogicTime = Time::ElapsedSinceStartup();
		}
		Profiler::Get().Begin();

		// main loop
		while( m_applicationShouldExit == false && m_window->IsOpen() == true )
		{
			for( int worldIndex = 0; worldIndex < m_worlds.size(); worldIndex++ )
			{
				EcsWorld& world = *m_worlds[worldIndex];

				Time& time = world.GetSingleton<Time>();
				const double currentTime = Time::ElapsedSinceStartup();

				// runs logic, renders ui
				while( currentTime > time.lastLogicTime + time.logicDelta )
				{
					// checking the loop timing is not late
					const double loopDelayMilliseconds = 1000. * ( currentTime - ( time.lastLogicTime + time.logicDelta ) );
					if( loopDelayMilliseconds > 30 )
					{
						Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
						// if we are really really late, resets the timer
						if( loopDelayMilliseconds > 100 )
						{
							time.lastLogicTime = currentTime - time.logicDelta;
							Debug::Warning() << "reset logic timer " << Debug::Endl();
						}
					}

					// increase the logic time of a timeScaleDelta with n timeScaleIncrements
					if( std::abs( time.timeScaleDelta ) >= time.timeScaleIncrement )
					{
						const float increment = time.timeScaleDelta > 0.f ? time.timeScaleIncrement : -time.timeScaleIncrement;
						time.lastLogicTime -= increment;
						time.timeScaleDelta -= increment;
					}

					time.lastLogicTime += time.logicDelta;

					SCOPED_PROFILE( logic );
					{
						SCOPED_PROFILE( init );
						Input::Get().NewFrame();
						Mouse::Get().Update( m_gameViewWindow->GetPosition(), m_gameViewWindow->GetSize(), m_gameViewWindow->IsHovered() );
						ImGui::NewFrame();
						ImGui::GetIO().DeltaTime = time.logicDelta;
						m_renderer->GetRendererDebug().ClearDebug();

						onLPPSynch.Emmit();
					}

					// update				
					{
						GameStep( world, time.logicDelta );

						EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();
						Scene& scene = world.GetSingleton<Scene>();

						// only update the editor camera when we are using it
						if( scene.mainCameraHandle == editorCamera.cameraHandle )
						{
							EditorCamera::Update( world, time.logicDelta );
						}
					}

					// ui & debug
					if( m_showUI )
					{
						{
							SCOPED_PROFILE( draw_ui );
							m_mainMenuBar->Draw();
							world.GetSingleton<EditorSelection>().Update( m_gameViewWindow->IsHovered() );
							world.Run<S_MoveFollowTransforms>();
							world.Run<S_MoveFollowTransformsUI>();
						}

						{
							SCOPED_PROFILE( debug_draw );
							EditorGrid::Draw( world.GetSingleton<EditorGrid>() );

							if( m_mainMenuBar->ShowWireframe() )
							{
								world.Run<S_DrawDebugWireframe>();
							}
							if( m_mainMenuBar->ShowNormals() )
							{
								world.Run<S_DrawDebugNormals>();
							}
							if( m_mainMenuBar->ShowAABB() )
							{
								world.Run<S_DrawDebugBounds>();
							}
							if( m_mainMenuBar->ShowHull() )
							{
								world.Run<S_DrawDebugHull>();
							}
							if( m_mainMenuBar->ShowLights() )
							{
								world.Run<S_DrawDebugPointLights>();
								world.Run<S_DrawDebugDirectionalLights>();
							}
						}
					}

					Input::Get().Manager().PullEvents();
					{
						// end frame
						SCOPED_PROFILE( scene_endFrame );
						world.ApplyTransitions();
					}

					{
						SCOPED_PROFILE( imgui_render );
						ImGui::Render();
					}
				}
			}

			// Render world		
			const double currentTime = Time::ElapsedSinceStartup();
			if( currentTime > lastRenderTime + Time::s_renderDelta )
			{
				lastRenderTime = currentTime;

				Time::RegisterFrameDrawn();	// used for stats

				UpdateRenderWorld( *m_renderer, GetCurrentWorld(), ToGLM( m_gameViewWindow->GetSize() ) );

				m_renderer->DrawFrame();
				Profiler::Get().End();
				Profiler::Get().Begin();
			}

			// sleep for the rest of the frame
			if( m_launchSettings.mainLoopSleep )
			{
				// @todo repair this to work with multiple worlds running 
// 				const double minSleepTime = 1;
// 				const double endFrameTime = Time::ElapsedSinceStartup();
// 				const double timeBeforeNextLogic = lastLogicTime + time.logicDelta - endFrameTime;
// 				const double timeBeforeNextRender = lastRenderTime + Time::s_renderDelta - endFrameTime;
// 				const double sleepTimeMiliseconds = 1000. * std::min( timeBeforeNextLogic, timeBeforeNextRender );
// 				if( sleepTimeMiliseconds > minSleepTime )
// 				{
// 					std::this_thread::sleep_for( std::chrono::milliseconds( int( sleepTimeMiliseconds / 2 ) ) );
// 				}
			}

		}

		// Exit sequence
		Debug::Log( "Exit application" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::GameStart( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::STOPPED )
		{
			// saves the scene before playing
			Scene& scene = _world.GetSingleton<Scene>();
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

			UseGameCamera( _world );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameStop( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PLAYING || game.state == Game::PAUSED )
		{
			UseEditorCamera( _world );

			Scene& scene = _world.GetSingleton<Scene>();

			// Saves the camera position for restoring it later
			const EcsEntity oldCameraID = _world.GetEntity( scene.mainCameraHandle );
			const btTransform oldCameraTransform = _world.GetComponent<Transform>( oldCameraID ).transform;

			// save old selection
			SceneNode* prevSelectionNode = _world.GetSingleton<EditorSelection>().GetSelectedSceneNode();
			const uint32_t prevSelectionHandle = prevSelectionNode != nullptr ? prevSelectionNode->handle : 0;

			Debug::Highlight() << game.name << ": stopped" << Debug::Endl();
			game.state = Game::STOPPED;
			if( game.gameServer != nullptr ) game.gameServer->Stop();
			else							 game.gameClient->Stop();

			scene.LoadFrom( scene.path ); // reload the scene 

			// restore camera transform
			const EcsEntity newCameraID = _world.GetEntity( scene.mainCameraHandle );
			_world.GetComponent<Transform>( newCameraID ).transform = oldCameraTransform;

			// restore selection
			if( prevSelectionHandle != 0 && scene.nodes.find( prevSelectionHandle ) != scene.nodes.end() )
			{
				fan::SceneNode& node = _world.GetComponent<fan::SceneNode>( _world.GetEntity( prevSelectionHandle ) );

				_world.GetSingleton<EditorSelection>().SetSelectedSceneNode( &node );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GamePause( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PLAYING )
		{
			Debug::Highlight() << game.name << ": paused" << Debug::Endl();
			game.state = Game::PAUSED;
		}
	}	

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameResume( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PAUSED )
		{
			Debug::Highlight() << game.name << ": resumed" << Debug::Endl();
			game.state = Game::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Editor::GameStep( EcsWorld& _world, float _delta )
	{
 		Game& game = _world.GetSingleton<Game>();
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
		EcsWorld& world = *_scene.world;
		world.GetSingleton<EditorSelection>().Deselect();
 		EditorCamera::CreateEditorCamera( world );
	}

	//================================================================================================================================
	// Updates the render world singleton component
	//================================================================================================================================
	void Editor::UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size )
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.targetSize = _size;

		// update render data
		_world.Run<S_UpdateRenderWorldModels>();
		_world.Run<S_UpdateRenderWorldUI>();
		_world.Run<S_UpdateRenderWorldPointLights>();
		_world.Run<S_UpdateRenderWorldDirectionalLights>();

		// particles mesh
		DrawMesh particlesDrawData;
		particlesDrawData.mesh = &renderWorld.particlesMesh;
		particlesDrawData.modelMatrix = glm::mat4( 1.f );
		particlesDrawData.normalMatrix = glm::mat4( 1.f );
		particlesDrawData.color = glm::vec4( 1.f, 1.f, 1.f, 1.f );
		particlesDrawData.shininess = 1;
		particlesDrawData.textureIndex = 1;
		renderWorld.drawData.push_back( particlesDrawData );

		_renderer.SetDrawData( renderWorld.drawData );
		_renderer.SetUIDrawData( renderWorld.uiDrawData );
		_renderer.SetPointLights( renderWorld.pointLights );
		_renderer.SetDirectionalLights( renderWorld.directionalLights );

		// Camera
		Scene& scene = _world.GetSingleton<Scene>();
		EcsEntity cameraID = _world.GetEntity( scene.mainCameraHandle );
		Camera& camera = _world.GetComponent<Camera>( cameraID );
		camera.aspectRatio = _size[0] / _size[1];
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
		_renderer.SetMainCamera(
			camera.GetProjection(),
			camera.GetView( cameraTransform ),
			ToGLM( cameraTransform.GetPosition() )
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::OnCurrentGameSwitchPlayStop()
	{
		EcsWorld& world = GetCurrentWorld();
		Game& game = world.GetSingleton<Game>();
		if( game.state == Game::STOPPED )
		{
			GameStart( world );
		}
		else
		{
			GameStop( world );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::UseEditorCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
 		scene.SetMainCamera( editorCamera.cameraHandle );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Editor::UseGameCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		scene.SetMainCamera( gameCamera.cameraHandle );
	}

	//================================================================================================================================
	// toogle the camera between editor and game
	//================================================================================================================================
	void Editor::OnCurrentGameToogleCamera()
	{
		EcsWorld& world = GetCurrentWorld();

		Game& game = world.GetSingleton<Game>();
		if( game.state == Game::STOPPED )
		{
			Debug::Warning() << "You cannot toogle camera outside of play mode" << Debug::Endl();
			return;
		}

		Scene& scene = world.GetSingleton<Scene>();
		EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();

		if( scene.mainCameraHandle == editorCamera.cameraHandle )
		{
			UseGameCamera( world );
		}
		else
		{
			UseEditorCamera( world );
		}
	}

	//================================================================================================================================
	// callback of the game view step button
	//================================================================================================================================
	void Editor::OnCurrentGameStep( )
	{
		EcsWorld& world = GetCurrentWorld();
		Game& game = world.GetSingleton<Game>();
		Time& time = world.GetSingleton<Time>();

		if( game.gameServer != nullptr )
		{
			game.gameServer->Step( time.logicDelta );
		}
		else
		{
			game.gameClient->Step( time.logicDelta );
		}
	}
}