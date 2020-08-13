#include <scene/singletons/fanRenderResources.hpp>
#include <scene/singletons/fanSceneResources.hpp>
#include "editor/fanEditorHolder.hpp"

#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderer.hpp"
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
#include "editor/fanMainMenuBar.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanDrawDebug.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/fanSceneTags.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanGame.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	EditorHolder::EditorHolder( const LaunchSettings _settings, std::vector<EcsWorld*> _gameWorlds ) :
		m_worlds( _gameWorlds )
		, m_applicationShouldExit( false )
		, m_launchSettings( _settings )
	{

		// Creates keyboard events
		Input::Get().Manager().CreateKeyboardEvent( "delete", Keyboard::DELETE );
		Input::Get().Manager().CreateKeyboardEvent( "open_scene", Keyboard::O, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "save_scene", Keyboard::S, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "reload_scene", Keyboard::R, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "freeze_capture", Keyboard::END );
		Input::Get().Manager().CreateKeyboardEvent( "copy", Keyboard::C, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "paste", Keyboard::V, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardEvent( "toogle_camera", Keyboard::TAB );
		Input::Get().Manager().CreateKeyboardEvent( "toogle_world", Keyboard::F1 );
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
		GameClient::CreateGameAxes();

		// creates window
		glm::ivec2 windowPosition;
		glm::ivec2 windowSize;
		SerializedValues::LoadWindowSizeAndPosition( _settings, windowPosition, windowSize );
		m_window.Create( _settings.windowName.c_str(), windowSize, windowPosition );
        Mouse::SetCallbacks( m_window.mWindow );

		// creates renderer
		m_renderer = new Renderer( m_window, Renderer::ViewType::Editor );
        RenderResources::SetupResources( m_renderer->mMeshManager,
                                         m_renderer->mMesh2DManager,
                                         m_renderer->mTextureManager );
        SceneResources::SetupResources( mPrefabManager );


		// Initialize editor components		
		mGameViewWindow    = new GameViewWindow( _settings.launchMode );
        mRenderWindow      = new RenderWindow( *m_renderer );
        mSceneWindow       = new SceneWindow();
        mInspectorWindow   = new InspectorWindow();
        mConsoleWindow     = new ConsoleWindow();
        mEcsWindow         = new EcsWindow();
        mProfilerWindow    = new ProfilerWindow();
        mPreferencesWindow = new PreferencesWindow( *m_renderer );
        mNetworkWindow     = new NetworkWindow();
        mSingletonsWindow  = new SingletonsWindow();
        mUnitTestsWindow   = new UnitTestsWindow();
        mMainMenuBar       = new MainMenuBar();
        mMainMenuBar->SetWindows( {
                mRenderWindow,
                mSceneWindow,
                mInspectorWindow,
                mConsoleWindow,
                mEcsWindow,
                mSingletonsWindow,
                mProfilerWindow,
                mGameViewWindow,
                mNetworkWindow,
                mPreferencesWindow,
                mUnitTestsWindow
        } );

		// Instance messages				
		mMainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, m_renderer );
		mMainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, m_renderer );
		mMainMenuBar->onExit.Connect( &EditorHolder::Exit, this );

		// Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, m_renderer );
		manager.FindEvent( "play_pause" )->Connect( &EditorHolder::OnCurrentGameSwitchPlayStop, this );
		manager.FindEvent( "copy" )->Connect( &EditorHolder::OnCurrentGameCopy, this );
		manager.FindEvent( "paste" )->Connect( &EditorHolder::OnCurrentGamePaste, this );
		manager.FindEvent( "show_ui" )->Connect( &EditorHolder::OnToogleShowUI, this );
		manager.FindEvent( "toogle_camera" )->Connect( &EditorHolder::OnCurrentGameToogleCamera, this );
		manager.FindEvent( "open_scene" )->Connect( &EditorHolder::OnCurrentGameOpen, this );
		manager.FindEvent( "save_scene" )->Connect( &EditorHolder::OnCurrentGameSave, this );
		manager.FindEvent( "reload_scene" )->Connect( &EditorHolder::OnCurrentGameReload, this );
		manager.FindEvent( "toogle_world" )->Connect( &EditorHolder::OnCycleCurrentGame, this );
		manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, m_renderer );

		mGameViewWindow->onSizeChanged.Connect( &Renderer::ResizeGame, m_renderer );
		mGameViewWindow->onPlay.Connect( &EditorHolder::OnCurrentGameStart, this );
		mGameViewWindow->onPause.Connect( &EditorHolder::OnCurrentGamePause, this );
		mGameViewWindow->onResume.Connect( &EditorHolder::OnCurrentGameResume, this );
		mGameViewWindow->onStop.Connect( &EditorHolder::OnCurrentGameStop, this );
		mGameViewWindow->onStep.Connect( &EditorHolder::OnCurrentGameStep, this );
		mGameViewWindow->onSelectGame.Connect( &EditorHolder::OnCurrentGameSelect, this );

        // Loop over all worlds to initialize them
		for (int worldIndex = 0; worldIndex < (int)m_worlds.size() ; worldIndex++)
		{
			assert( m_worlds[worldIndex] != nullptr );
			EcsWorld& world = *m_worlds[worldIndex];

			world.AddSingletonType<EditorCamera>();
			world.AddSingletonType<EditorGrid>();
			world.AddSingletonType<EditorSelection>();
			world.AddSingletonType<EditorCopyPaste>();
			world.AddSingletonType<EditorGizmos>();
			world.AddTagType<tag_editorOnly>();

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.isHeadless = ( &world != &GetCurrentWorld() );

            RenderResources& renderResources = world.GetSingleton<RenderResources>();
            renderResources.SetPointers( &m_renderer->mMeshManager,
                                         &m_renderer->mMesh2DManager,
                                         &m_renderer->mTextureManager );

            SceneResources& sceneResources = world.GetSingleton<SceneResources>();
            sceneResources.SetPointers( &mPrefabManager );

			Scene& scene = world.GetSingleton<Scene>();
			EditorSelection& selection = world.GetSingleton<EditorSelection>();

			selection.ConnectCallbacks( scene );
			mSceneWindow->onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );

			scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, mSceneWindow );
			scene.onLoad.Connect( &EditorHolder::OnSceneLoad, this );

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

	//========================================================================================================
	//========================================================================================================
	EditorHolder::~EditorHolder()
	{
		// Deletes ui
		delete mMainMenuBar;

		// Serialize editor positions if it was not modified by a launch command
		if( m_launchSettings.window_size == glm::ivec2( -1, -1 ) )
		{
			const VkExtent2D rendererSize = m_window.GetExtent();
			const glm::ivec2 windowPosition = m_window.GetPosition();
			SerializedValues::Get().SetUInt( "renderer_extent_width", rendererSize.width );
			SerializedValues::Get().SetUInt( "renderer_extent_height", rendererSize.height );
			SerializedValues::Get().SetInt( "renderer_position_x", windowPosition.x );
			SerializedValues::Get().SetInt( "renderer_position_y", windowPosition.y );
		}

		SerializedValues::Get().SaveValuesToDisk();

		mPrefabManager.Clear();

		delete m_renderer;
		m_window.Destroy();
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::Exit()
	{
		m_applicationShouldExit = true;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::Run()
	{
		// initializes timers
		m_lastRenderTime = Time::ElapsedSinceStartup();
		for( EcsWorld* world : m_worlds )
		{
			Time& time = world->GetSingleton<Time>();
			time.lastLogicTime = Time::ElapsedSinceStartup();
		}
		Profiler::Get().Begin();

		// main loop
		while( m_applicationShouldExit == false && m_window.IsOpen() == true )
		{
			Step();
		}

		// Exit sequence
		Debug::Log( "Exit application" );
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::Step()
	{
		const double currentTime = Time::ElapsedSinceStartup();
		const bool renderIsThisFrame = currentTime > m_lastRenderTime + Time::s_renderDelta;
		const Time& currentWorldTime = GetCurrentWorld().GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      currentWorldTime.lastLogicTime + currentWorldTime.logicDelta;

		// Update all worlds
		for( int worldIndex = 0; worldIndex < (int)m_worlds.size(); worldIndex++ )
		{
			EcsWorld& world = *m_worlds[worldIndex];
			const bool isCurrentWorld = ( &world == &GetCurrentWorld() );			

			// runs logic, renders ui
			Time& time = world.GetSingleton<Time>();
			while ( currentTime > time.lastLogicTime + time.logicDelta )
			{
				world.GetSingleton<RenderDebug>().Clear();

				if( isCurrentWorld )
				{
					// Update input
					ImGui::GetIO().DeltaTime = time.logicDelta;
					const btVector2 gamePos  = mGameViewWindow->GetPosition();
                    const btVector2 gameSize = mGameViewWindow->GetSize();
                    //todo mGameViewWindow->IsHovered()
                    Mouse::NextFrame( m_window.mWindow, ToGLM( gamePos), ToGLM( gameSize) );
                    Input::Get().NewFrame();
					Input::Get().Manager().PullEvents();
                    world.GetSingleton<Mouse>().UpdateData( m_window.mWindow );

				}

				// checking the loop timing is not late
                const double loopDelayMilliseconds = 1000. *
                                                     ( currentTime -
                                                     ( time.lastLogicTime + time.logicDelta ) );
                if( loopDelayMilliseconds > 30 )
                {
                    //Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
                    // if we are really really late, resets the timer
                    if( loopDelayMilliseconds > 100 )
                    {
                        time.lastLogicTime = currentTime - time.logicDelta;
                        //Debug::Warning() << "reset logic timer " << Debug::Endl();
                    }
                }

                // increase the logic time of a timeScaleDelta with n timeScaleIncrements
                if( std::abs( time.timeScaleDelta ) >= time.timeScaleIncrement )
                {
                    const float increment = time.timeScaleDelta > 0.f
                            ? time.timeScaleIncrement
                            : -time.timeScaleIncrement;
					time.lastLogicTime -= increment;
					time.timeScaleDelta -= increment;
				}

				time.lastLogicTime += time.logicDelta;		

				GameStep( world, time.logicDelta );							

				// ui & debug
				if( m_showUI )
				{					
					world.Run<SMoveFollowTransforms>();
				}

				if( isCurrentWorld )
				{
					assert( logicIsThisFrame );
					EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();
					Scene& scene = world.GetSingleton<Scene>();
					// only update the editor camera when we are using it
					if( scene.mainCameraHandle == editorCamera.cameraHandle )
					{
						EditorCamera::Update( world, time.logicDelta );
					}
					world.GetSingleton<EditorSelection>().Update( mGameViewWindow->IsHovered() );

					if( renderIsThisFrame )
					{
                        SCOPED_PROFILE( debug_draw );
						// Debug Draw
						if( mMainMenuBar->ShowWireframe() ) { world.Run<S_DrawDebugWireframe>(); }
						if( mMainMenuBar->ShowNormals() ) { world.Run<S_DrawDebugNormals>(); }
						if( mMainMenuBar->ShowAABB() ) { world.Run<S_DrawDebugBounds>(); }
						if( mMainMenuBar->ShowHull() ) { world.Run<S_DrawDebugHull>(); }
						if( mMainMenuBar->ShowLights() )
						{
							world.Run<S_DrawDebugPointLights>();
							world.Run<S_DrawDebugDirectionalLights>();
						}
						EditorGrid::Draw( GetCurrentWorld() );

						// ImGui render
                        {
                            SCOPED_PROFILE( ImGui_render );
						    ImGui::NewFrame();
						    mMainMenuBar->Draw( world );
						    ImGui::Render();
                        }
					}
				}
                {
                    SCOPED_PROFILE( apply_transitions );
				    world.ApplyTransitions();
                }
			}
		}

        onLPPSynch.Emmit();

		// Render world		
		if( renderIsThisFrame && logicIsThisFrame )
		{
		    EcsWorld& currentWorld = GetCurrentWorld();
			m_lastRenderTime = currentTime;

			Time::RegisterFrameDrawn();	// used for stats

			UpdateRenderWorld( *m_renderer, currentWorld, ToGLM( mGameViewWindow->GetSize() ) );

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

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::GameStart( EcsWorld& _world )
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

	//========================================================================================================
	//========================================================================================================
	void  EditorHolder::GameStop( EcsWorld& _world )
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
				fan::SceneNode& node = _world.GetComponent<fan::SceneNode>
				                                        ( _world.GetEntity( prevSelectionHandle ) );

				_world.GetSingleton<EditorSelection>().SetSelectedSceneNode( &node );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  EditorHolder::GamePause( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PLAYING )
		{
			Debug::Highlight() << game.name << ": paused" << Debug::Endl();
			game.state = Game::PAUSED;
		}
	}	

	//========================================================================================================
	//========================================================================================================
	void  EditorHolder::GameResume( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PAUSED )
		{
			Debug::Highlight() << game.name << ": resumed" << Debug::Endl();
			game.state = Game::PLAYING;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void  EditorHolder::GameStep( EcsWorld& _world, float _delta )
	{
        SCOPED_PROFILE(game_step );
 		Game& game = _world.GetSingleton<Game>();
 		const float delta = ( game.state == Game::PLAYING ? _delta : 0.f );
 		if( game.gameServer != nullptr ) { game.gameServer->Step( delta ); }
 		else							 { game.gameClient->Step( delta ); }

	}

	//========================================================================================================
	// Todo save camera position depending on the scene
	//========================================================================================================
	void EditorHolder::OnSceneLoad( Scene& _scene )
	{
		EcsWorld& world = *_scene.world;		
 		EditorCamera::CreateEditorCamera( world );

		if( &world == &GetCurrentWorld() )
		{
			world.GetSingleton<EditorSelection>().Deselect();
		}
	}

	//========================================================================================================
	// Updates the render world singleton component
	//========================================================================================================
	void EditorHolder::UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size )
	{
        SCOPED_PROFILE( update_RW );
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		const RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
		renderWorld.targetSize = _size;

		// update render data
        {
            SCOPED_PROFILE( update_render_data );
            _world.Run<SUpdateRenderWorldModels>();
            _world.Run<SUpdateRenderWorldUI>();
            _world.Run<S_UpdateRenderWorldPointLights>();
            _world.Run<S_UpdateRenderWorldDirectionalLights>();
        }

		// particles mesh
		RenderDataModel particlesDrawData;
		particlesDrawData.mesh = renderWorld.particlesMesh;
		particlesDrawData.modelMatrix = glm::mat4( 1.f );
		particlesDrawData.normalMatrix = glm::mat4( 1.f );
		particlesDrawData.color = glm::vec4( 1.f, 1.f, 1.f, 1.f );
		particlesDrawData.shininess = 1;
		particlesDrawData.textureIndex = 1;
		renderWorld.drawData.push_back( particlesDrawData );

        {
            SCOPED_PROFILE(set_render_data);
            _renderer.SetDrawData(renderWorld.drawData);
            _renderer.SetUIDrawData(renderWorld.uiDrawData);
            _renderer.SetPointLights(renderWorld.pointLights);
            _renderer.SetDirectionalLights(renderWorld.directionalLights);
            _renderer.SetDebugDrawData( renderDebug.debugLines,
                                        renderDebug.debugLinesNoDepthTest,
                                        renderDebug.debugTriangles );
        }

		// Camera
        {
            SCOPED_PROFILE( update_camera );
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
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::OnCycleCurrentGame()
	{
		OnCurrentGameSelect( ( m_currentWorld + 1 ) % ( m_worlds.size() ) );
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::OnCurrentGameSwitchPlayStop()
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

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::UseEditorCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
 		scene.SetMainCamera( editorCamera.cameraHandle );
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::UseGameCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();
		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		scene.SetMainCamera( gameCamera.cameraHandle );
	}

	//========================================================================================================
	//========================================================================================================
	void EditorHolder::OnCurrentGameOpen() { mMainMenuBar->Open( GetCurrentWorld() ); }
	void EditorHolder::OnCurrentGameReload() { mMainMenuBar->Reload( GetCurrentWorld() ); }
	void EditorHolder::OnCurrentGameSave() { mMainMenuBar->Save( GetCurrentWorld() ); }
	void EditorHolder::OnCurrentGameCopy() { GetCurrentWorld().GetSingleton<EditorCopyPaste>().OnCopy(); }
	void EditorHolder::OnCurrentGamePaste() { GetCurrentWorld().GetSingleton<EditorCopyPaste>().OnPaste(); }

	//========================================================================================================
	// sets which ecs world will be displayed in the editor
	//========================================================================================================
	void EditorHolder::OnCurrentGameSelect( const int _index )
	{
		assert( _index < (int)m_worlds.size() );

		m_currentWorld = _index;

		// Set all to headless except the current
		for( int worldIndex = 0; worldIndex < (int)m_worlds.size(); worldIndex++ )
		{
			EcsWorld& world = *m_worlds[worldIndex];
			RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
			renderWorld.isHeadless = ( worldIndex != m_currentWorld );
		}

		mGameViewWindow->SetCurrentGameSelected( m_currentWorld );
	}

	//========================================================================================================
	// toogle the camera between editor and game
	//========================================================================================================
	void EditorHolder::OnCurrentGameToogleCamera()
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

	//========================================================================================================
	// callback of the game view step button
	//========================================================================================================
	void EditorHolder::OnCurrentGameStep( )
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