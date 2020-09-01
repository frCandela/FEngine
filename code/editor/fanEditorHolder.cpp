#include <scene/singletons/fanRenderResources.hpp>
#include <scene/singletons/fanSceneResources.hpp>
#include <editor/singletons/fanEditorPlayState.hpp>
#include "editor/fanEditorHolder.hpp"

#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "editor/windows/fanPreferencesWindow.hpp"	
#include "editor/windows/fanUnitsTestsWindow.hpp"
#include "editor/windows/fanSingletonsWindow.hpp"
#include "editor/windows/fanInspectorWindow.hpp"
#include "editor/windows/fanProfilerWindow.hpp"
#include "editor/windows/fanGameViewWindow.hpp"
#include "editor/windows/fanConsoleWindow.hpp"
#include "editor/windows/fanRenderWindow.hpp"
#include "editor/windows/fanSceneWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanDrawDebug.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    EditorHolder::EditorHolder( LaunchSettings& _settings, const std::vector<IGame*>& _games ) :
            IHolder( AdaptSettings(_settings) ),
            mGames( _games )
    {
        for( IGame* gameBase : mGames )
        {
            fanAssert( gameBase != nullptr );
            gameBase->Init();
        }

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

        // Initialize editor components
        mMainMenuBar    = new MainMenuBar();
        mGameViewWindow = new GameViewWindow( _settings.launchMode );
        SceneWindow& sceneWindow = *new SceneWindow();
        mMainMenuBar->SetWindows( {
            mGameViewWindow,
            &sceneWindow,
            new RenderWindow( mRenderer ),
            new InspectorWindow(),
            new ConsoleWindow(),
            new EcsWindow(),
            new ProfilerWindow(),
            new PreferencesWindow( mRenderer, mFullScreen ),
            new SingletonsWindow(),
            new UnitTestsWindow()
        } );

        // Instance messages
        mMainMenuBar->mOnReloadShaders.Connect( &Renderer::ReloadShaders, &mRenderer );
        mMainMenuBar->mOnReloadIcons.Connect( &Renderer::ReloadIcons, &mRenderer );
        mMainMenuBar->mOnExit.Connect( &IHolder::Exit, (IHolder*)this );

        // Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, &mRenderer );
        manager.FindEvent( "play_pause" )->Connect( &EditorHolder::OnCurrentGameSwitchPlayStop, this );
        manager.FindEvent( "copy" )->Connect( &EditorHolder::OnCurrentGameCopy, this );
        manager.FindEvent( "paste" )->Connect( &EditorHolder::OnCurrentGamePaste, this );
        manager.FindEvent( "show_ui" )->Connect( &EditorHolder::OnToogleShowUI, this );
        manager.FindEvent( "toogle_camera" )->Connect( &EditorHolder::OnCurrentGameToogleCamera, this );
        manager.FindEvent( "open_scene" )->Connect( &EditorHolder::OnCurrentGameOpen, this );
        manager.FindEvent( "save_scene" )->Connect( &EditorHolder::OnCurrentGameSave, this );
        manager.FindEvent( "reload_scene" )->Connect( &EditorHolder::OnCurrentGameReload, this );
        manager.FindEvent( "toogle_world" )->Connect( &EditorHolder::OnCycleCurrentGame, this );
        manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, &mRenderer );
        manager.FindEvent( "delete" )->Connect( &EditorHolder::OnCurrentGameDeleteSelection, this );
        manager.FindEvent( "toogle_follow_transform_lock" )->Connect( &EditorHolder::OnCurrentGameToogleTransformLock, this );

        mGameViewWindow->mOnSizeChanged.Connect( &Renderer::ResizeGame, &mRenderer );
        mGameViewWindow->mOnPlay.Connect( &EditorHolder::OnCurrentGameStart, this );
        mGameViewWindow->mOnPause.Connect( &EditorHolder::OnCurrentGamePause, this );
        mGameViewWindow->mOnResume.Connect( &EditorHolder::OnCurrentGameResume, this );
        mGameViewWindow->mOnStop.Connect( &EditorHolder::OnCurrentGameStop, this );
        mGameViewWindow->mOnStep.Connect( &EditorHolder::OnCurrentGameStep, this );
        mGameViewWindow->mOnSelectGame.Connect( &EditorHolder::OnCurrentGameSelect, this );

        // Loop over all worlds to initialize them
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame   & game  = *mGames[gameIndex];
            EcsWorld& world = game.mWorld;

            world.AddSingletonType<EditorPlayState>();
            world.AddSingletonType<EditorCamera>();
            world.AddSingletonType<EditorGrid>();
            world.AddSingletonType<EditorSelection>();
            world.AddSingletonType<EditorCopyPaste>();
            world.AddSingletonType<EditorGizmos>();
            world.AddTagType<TagEditorOnly>();

            InitWorld( world );

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( &game != &GetCurrentGame() );

            Scene          & scene     = world.GetSingleton<Scene>();
            EditorSelection& selection = world.GetSingleton<EditorSelection>();

            selection.ConnectCallbacks( scene );
            sceneWindow.onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );

            scene.mOnLoad.Connect( &SceneWindow::OnExpandHierarchy, &sceneWindow );
            scene.mOnLoad.Connect( &EditorHolder::OnSceneLoad, this );

            // load scene
            scene.New();
            if( !_settings.loadScene.empty() )
            {
                scene.LoadFrom( _settings.loadScene );

                // auto play the scene
                if( _settings.autoPlay )
                {
                    GameStart( game );
                }
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    LaunchSettings& EditorHolder::AdaptSettings( LaunchSettings& _settings )
    {
        if( ! _settings.mForceWindowDimensions )
        {
            SerializedValues::LoadWindowPosition( _settings.window_position );
            SerializedValues::LoadWindowSize( _settings.window_size );
        }
        _settings.mIconPath = RenderGlobal::sEditorIcon;
        return _settings;
    }

    //========================================================================================================
    //========================================================================================================
    EditorHolder::~EditorHolder()
    {
        // Deletes ui
        delete mMainMenuBar;
        if( ! mLaunchSettings.mForceWindowDimensions )
        {
            SerializedValues::SaveWindowPosition( mWindow.GetPosition() );
            SerializedValues::SaveWindowSize( mWindow.GetSize() );
        }
        SerializedValues::Get().SaveValuesToDisk();
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::Run()
    {
        // initializes timers
        mLastRenderTime = Time::ElapsedSinceStartup();
        for( IGame* game : mGames )
        {
            Time& time = game->mWorld.GetSingleton<Time>();
            time.mLastLogicTime = Time::ElapsedSinceStartup();
        }
        Profiler::Get().Begin();

        // main loop
        while( mApplicationShouldExit == false && mWindow.IsOpen() == true )
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
        const double currentTime       = Time::ElapsedSinceStartup();
        const bool   renderIsThisFrame = currentTime > mLastRenderTime + Time::sRenderDelta;
        const Time& currentWorldTime = GetCurrentGame().mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      currentWorldTime.mLastLogicTime + currentWorldTime.mLogicDelta;

        // Update all worlds
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame   & game  = *mGames[gameIndex];
            EcsWorld& world = game.mWorld;
            const bool isCurrentWorld = ( &game == &GetCurrentGame() );

            // runs logic, renders ui
            Time& time = world.GetSingleton<Time>();
            while( currentTime > time.mLastLogicTime + time.mLogicDelta )
            {
                world.GetSingleton<RenderDebug>().Clear();

                if( isCurrentWorld )
                {
                    // Update input
                    ImGui::GetIO().DeltaTime = time.mLogicDelta;
                    const btVector2 gamePos  = mGameViewWindow->GetPosition();
                    const btVector2 gameSize = mGameViewWindow->GetSize();
                    //todo mGameViewWindow->IsHovered()
                    Mouse::NextFrame( mWindow.mWindow, ToGLM( gamePos ), ToGLM( gameSize ) );
                    Input::Get().NewFrame();
                    Input::Get().Manager().PullEvents();
                    world.GetSingleton<Mouse>().UpdateData( mWindow.mWindow );
                }

                // checking the loop timing is not late
                const double loopDelayMilliseconds = 1000. *
                                                     ( currentTime -
                                                       ( time.mLastLogicTime + time.mLogicDelta ) );
                if( loopDelayMilliseconds > 30 )
                {
                    //Debug::Warning() << "logic is late of " << loopDelayMilliseconds
                    //                 << "ms" << Debug::Endl();
                    // if we are really really late, resets the timer
                    if( loopDelayMilliseconds > 100 )
                    {
                        time.mLastLogicTime = currentTime - time.mLogicDelta;
                        //Debug::Warning() << "reset logic timer " << Debug::Endl();
                    }
                }

                // increase the logic time of a timeScaleDelta with n timeScaleIncrements
                if( std::abs( time.mTimeScaleDelta ) >= time.mTimeScaleIncrement )
                {
                    const float increment = time.mTimeScaleDelta > 0.f
                            ? time.mTimeScaleIncrement
                            : -time.mTimeScaleIncrement;
                    time.mLastLogicTime -= increment;
                    time.mTimeScaleDelta -= increment;
                }

                time.mLastLogicTime += time.mLogicDelta;

                const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
                game.Step( playState.mState == EditorPlayState::PLAYING ? time.mLogicDelta : 0.f );

                // ui & debug
                if( mShowUi )
                {
                    world.Run<SMoveFollowTransforms>();
                }

                if( isCurrentWorld )
                {
                    fanAssert( logicIsThisFrame );
                    EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();
                    Scene       & scene        = world.GetSingleton<Scene>();
                    // only update the editor camera when we are using it
                    if( scene.mMainCameraHandle == editorCamera.mCameraHandle )
                    {
                        EditorCamera::Update( world, time.mLogicDelta );
                    }
                    world.GetSingleton<EditorSelection>().Update( mGameViewWindow->IsHovered() );

                    if( renderIsThisFrame )
                    {
                        SCOPED_PROFILE( debug_draw );
                        // Debug Draw
                        if( mMainMenuBar->ShowWireframe() ){ world.Run<SDrawDebugWireframe>(); }
                        if( mMainMenuBar->ShowNormals() ){ world.Run<SDrawDebugNormals>(); }
                        if( mMainMenuBar->ShowAABB() ){ world.Run<SDrawDebugBounds>(); }
                        if( mMainMenuBar->ShowHull() ){ world.Run<SDrawDebugHull>(); }
                        if( mMainMenuBar->ShowUiBounds() ){ world.Run<SDrawDebugUiBounds>(); }
                        if( mMainMenuBar->ShowLights() )
                        {
                            world.Run<SDrawDebugPointLights>();
                            world.Run<SDrawDebugDirectionalLights>();
                        }
                        EditorGrid::Draw( GetCurrentGame().mWorld );

                        // ImGui render
                        {
                            SCOPED_PROFILE( ImGui_render );
                            ImGui::NewFrame();
                            mMainMenuBar->Draw( game.mWorld );
                            /*if( ImGui::Begin( "test" ) )
                            {
                                ImGui::End();
                            }*/
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

        mOnLPPSynch.Emmit();

        // Render world
        if( renderIsThisFrame && logicIsThisFrame )
        {
            mLastRenderTime = currentTime;

            Time::RegisterFrameDrawn();    // used for stats


            UpdateRenderWorld( mRenderer, GetCurrentGame(), ToGLM( mGameViewWindow->GetSize() ) );

            mRenderer.DrawFrame();
            Profiler::Get().End();
            Profiler::Get().Begin();
        }

        // sleep for the rest of the frame
        if( mLaunchSettings.mainLoopSleep )
        {
            // @todo repair this to work with multiple worlds running
// 			const double minSleepTime = 1;
// 			const double endFrameTime = Time::ElapsedSinceStartup();
// 			const double timeBeforeNextLogic = lastLogicTime + time.logicDelta - endFrameTime;
// 			const double timeBeforeNextRender = lastRenderTime + Time::s_renderDelta - endFrameTime;
// 			const double sleepTimeMiliseconds = 1000. * std::min( timeBeforeNextLogic, timeBeforeNextRender );
// 			if( sleepTimeMiliseconds > minSleepTime )
// 			{
// 				std::this_thread::sleep_for( std::chrono::milliseconds( int( sleepTimeMiliseconds / 2 ) ) );
// 			}
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameStart( IGame& _game )
    {
        // saves the scene before playing
        Scene& scene = _game.mWorld.GetSingleton<Scene>();
        if( scene.mPath.empty() )
        {
            Debug::Warning() << "please save the scene before playing" << Debug::Endl();
            return;
        }

        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            scene.Save();
            Debug::Highlight() << _game.mName << ": start" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
            _game.Start();
        }
    }

    //========================================================================================================
    //========================================================================================================
    struct SceneRestoreState
    {
        SceneRestoreState( Scene& _scene ) : mScene( _scene ) {}
        void Save()
        {
            EcsWorld& world = *mScene.mWorld;

            // Saves the camera position for restoring it later
            const EcsEntity oldCameraID = world.GetEntity( mScene.mMainCameraHandle );
            mPrevCameraTransform = world.GetComponent<Transform>( oldCameraID ).mTransform;
            // save old selection
            SceneNode* prevSelectionNode = world.GetSingleton<EditorSelection>().GetSelectedSceneNode();
            mPrevSelectionHandle = prevSelectionNode != nullptr ? prevSelectionNode->mHandle : 0;
        }

        void Restore() const
        {
            EcsWorld& world = *mScene.mWorld;

            // restore camera transform
            const EcsEntity newCameraID = world.GetEntity( mScene.mMainCameraHandle );
            world.GetComponent<Transform>( newCameraID ).mTransform = mPrevCameraTransform;

            // restore selection
            if( mPrevSelectionHandle != 0 &&
                mScene.mNodes.find( mPrevSelectionHandle ) != mScene.mNodes.end() )
            {
                fan::SceneNode& node = world.GetComponent<fan::SceneNode>
                        ( world.GetEntity( mPrevSelectionHandle ) );

                world.GetSingleton<EditorSelection>().SetSelectedSceneNode( &node );
            }
        }

        Scene& mScene;
        EcsHandle   mPrevSelectionHandle = 0;
        btTransform mPrevCameraTransform;
    };

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameStop( IGame& _game )
    {
        EcsWorld& world = _game.mWorld;

        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING || playState.mState == EditorPlayState::PAUSED )
        {
            UseEditorCamera( world );

            Scene& scene = world.GetSingleton<Scene>();
            SceneRestoreState restoreState( scene );
            restoreState.Save();

            Debug::Highlight() << _game.mName << ": stopped" << Debug::Endl();
            playState.mState = EditorPlayState::STOPPED;
            _game.Stop();
            scene.LoadFrom( scene.mPath ); // reload the scene

            restoreState.Restore();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GamePause( IGame& _game )
    {
        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING )
        {
            Debug::Highlight() << _game.mName << ": paused" << Debug::Endl();
            playState.mState = EditorPlayState::PAUSED;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameResume( IGame& _game )
    {
        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PAUSED )
        {
            Debug::Highlight() << _game.mName << ": resumed" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnSceneLoad( Scene& _scene )
    {
        EcsWorld& world = *_scene.mWorld;
        EditorCamera::CreateEditorCamera( world );

        if( &world == &GetCurrentGame().mWorld )
        {
            world.GetSingleton<EditorSelection>().Deselect();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCycleCurrentGame()
    {
        OnCurrentGameSelect( ( mCurrentGame + 1 ) % ( mGames.size() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameStart()
    {
        IGame& game = GetCurrentGame();
        GameStart( game );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameSwitchPlayStop()
    {
        IGame                & game      = GetCurrentGame();
        const EditorPlayState& playState = game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            GameStart( game );
        }
        else
        {
            GameStop( game );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::UseEditorCamera( EcsWorld& _world )
    {
        Scene       & scene        = _world.GetSingleton<Scene>();
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        scene.SetMainCamera( editorCamera.mCameraHandle );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameOpen() { mMainMenuBar->Open( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameReload() { mMainMenuBar->Reload( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameSave() { mMainMenuBar->Save( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameCopy()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorCopyPaste>().OnCopy();
    }
    void EditorHolder::OnCurrentGamePaste()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorCopyPaste>().OnPaste();
    }
    void EditorHolder::OnCurrentGameDeleteSelection()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorSelection>().DeleteSelection();
    }
    void EditorHolder::OnCurrentGameToogleTransformLock()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorSelection>().OnToogleTransformLock();
    }

    //========================================================================================================
    // sets which ecs world will be displayed in the editor
    //========================================================================================================
    void EditorHolder::OnCurrentGameSelect( const int _index )
    {
        fanAssert( _index < (int)mGames.size() );

        mCurrentGame = _index;

        // Set all to headless except the current
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame      & game        = *mGames[gameIndex];
            RenderWorld& renderWorld = game.mWorld.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( gameIndex != mCurrentGame );
        }

        mGameViewWindow->SetCurrentGameSelected( mCurrentGame );
    }

    //========================================================================================================
    // toogle the camera between editor and game
    //========================================================================================================
    void EditorHolder::OnCurrentGameToogleCamera()
    {
        IGame   & currentGame = GetCurrentGame();
        EcsWorld& world       = currentGame.mWorld;

        const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            Debug::Warning() << "You cannot toogle camera outside of play mode" << Debug::Endl();
            return;
        }

        Scene       & scene        = world.GetSingleton<Scene>();
        EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();

        if( scene.mMainCameraHandle == editorCamera.mCameraHandle )
        {
            world.GetSingleton<Scene>().mOnEditorUseGameCamera.Emmit();
        }
        else
        {
            UseEditorCamera( world );
        }
    }

    //========================================================================================================
    // callback of the game view step button
    //========================================================================================================
    void EditorHolder::OnCurrentGameStep()
    {
        IGame& game = GetCurrentGame();
        Time & time = game.mWorld.GetSingleton<Time>();
        game.Step( time.mLogicDelta );
    }
}