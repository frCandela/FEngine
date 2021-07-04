#include "editor/fanEditorPlayer.hpp"

#include "platform/input/fanInputManager.hpp"
#include "platform/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"

#include "engine/game/fanIGame.hpp"
#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/physics/fanUpdateRigidbodies.hpp"
#include "engine/physics/fanDetectCollisions.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanDrawDebug.hpp"
#include "engine/systems/fanUpdateAnimators.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"

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
#include "editor/windows/fanTerrainWindow.hpp"

#include "editor/singletons/fanEditorMainMenuBar.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EditorPlayer::EditorPlayer( LaunchSettings& _settings, const std::vector<IGame*>& _games ) :
            mData( AdaptSettings( _settings ) ),
            mGames( _games ),
            mLastLogicFrameRendered( 0 )
    {
        EditorSettingsData::LoadJsonFromDisk( mEditorSettings.mJson );
        EditorSettingsData::LoadSettingsFromJson( mEditorSettings );

        // Creates keyboard events
        Input::Get().Manager().CreateKeyboardEvent( "delete", Keyboard::DEL );
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
        Input::Get().Manager().CreateKeyboardAxis( "editor_boost", Keyboard::LEFT_SHIFT, Keyboard::LEFT_CONTROL );

        // Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, &mData.mRenderer );
        manager.FindEvent( "play_pause" )->Connect( &EditorPlayer::OnSwitchPlayStop, this );
        manager.FindEvent( "copy" )->Connect( &EditorPlayer::OnCopy, this );
        manager.FindEvent( "paste" )->Connect( &EditorPlayer::OnPaste, this );
        manager.FindEvent( "show_ui" )->Connect( &EditorPlayer::OnToggleShowUI, this );
        manager.FindEvent( "toogle_camera" )->Connect( &EditorPlayer::OnToogleCamera, this );
        manager.FindEvent( "open_scene" )->Connect( &EditorPlayer::OnOpen, this );
        manager.FindEvent( "save_scene" )->Connect( &EditorPlayer::OnSave, this );
        manager.FindEvent( "reload_scene" )->Connect( &EditorPlayer::OnReload, this );
        manager.FindEvent( "toogle_world" )->Connect( &EditorPlayer::OnCycleCurrentGame, this );
        manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, &mData.mRenderer );
        manager.FindEvent( "delete" )->Connect( &EditorPlayer::OnDeleteSelection, this );
        manager.FindEvent( "toogle_follow_transform_lock" )->Connect( &EditorPlayer::OnToogleTransformLock, this );

        // Loop over all worlds to initialize them
        for( int i = 0; i < (int)mGames.size(); i++ )
        {
            IGame   & game  = *mGames[i];
            EcsWorld& world = game.GetWorld();

            PlayerData::EcsIncludeEngine( world );
            PlayerData::EcsIncludePhysics( world );
            PlayerData::EcsIncludeRender3D( world );
            PlayerData::EcsIncludeRenderUI( world );
            EcsIncludeEditor( world );

            EditorSettings& editorSerializedValues = world.GetSingleton<EditorSettings>();
            editorSerializedValues.mData = &mEditorSettings;

            world.GetSingleton<Application>().Setup( &mData.mResources );

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( &game != &GetCurrentGame() );

            Scene          & scene     = world.GetSingleton<Scene>();
            EditorSelection& selection = world.GetSingleton<EditorSelection>();

            selection.ConnectCallbacks( scene );

            {
                EditorMainMenuBar& mainMenuBar = world.GetSingleton<EditorMainMenuBar>();
                mainMenuBar.mOnReloadShaders.Connect( &Renderer::ReloadShaders, &mData.mRenderer );
                mainMenuBar.mOnReloadIcons.Connect( &Renderer::ReloadIcons, &mData.mRenderer );
                mainMenuBar.mOnExit.Connect( &EditorPlayer::Exit, this );

                GameViewWindow& gameViewWindow = world.GetSingleton<GameViewWindow>();
                gameViewWindow.mOnSizeChanged.Connect( &Renderer::ResizeGame, &mData.mRenderer );
                gameViewWindow.mOnPlay.Connect( &EditorPlayer::OnStart, this );
                gameViewWindow.mOnPause.Connect( &EditorPlayer::OnPause, this );
                gameViewWindow.mOnResume.Connect( &EditorPlayer::OnResume, this );
                gameViewWindow.mOnStop.Connect( &EditorPlayer::OnStop, this );
                gameViewWindow.mOnStep.Connect( &EditorPlayer::OnStep, this );
                gameViewWindow.mOnSelectGame.Connect( &EditorPlayer::OnSelect, this );

                SceneWindow& sceneWindow = world.GetSingleton<SceneWindow>();
                sceneWindow.onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );
                scene.mOnLoad.Connect( &SceneWindow::OnExpandHierarchy, &sceneWindow );
                scene.mOnLoad.Connect( &EditorPlayer::OnSceneLoad, this );
            }
            game.Init();
            world.PostInitSingletons( true );

            // load scene
            scene.New();
            if( !_settings.mLoadScene.empty() )
            {
                scene.LoadFrom( _settings.mLoadScene );

                // auto play the scene
                if( _settings.mAutoPlay )
                {
                    Start( game );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    LaunchSettings& EditorPlayer::AdaptSettings( LaunchSettings& _settings )
    {
        if( !_settings.mForceWindowDimensions )
        {
            Json json;
            EditorSettingsData::LoadJsonFromDisk( json );
            EditorSettingsData::LoadWindowSizeAndPosition( json, _settings.mWindow_size, _settings.mWindow_position );
        }
        _settings.mIconPath     = RenderGlobal::sEditorIcon;
        _settings.mLaunchEditor = true;
        return _settings;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Run()
    {
        Profiler::Get().Begin();
        while( mData.mApplicationShouldExit == false && mData.mWindow.IsOpen() == true )        // main loop
        {
            Step();
        }

        Debug::Log( "Exit application", Debug::Type::Editor );

        EditorSettingsData::SaveSettingsToJson( mEditorSettings );
        EditorSettingsData::SaveWindowSizeAndPosition( mEditorSettings.mJson, mData.mRenderer.mWindow.GetSize(), mData.mRenderer.mWindow.GetPosition() );
        EditorSettingsData::SaveJsonToDisk( mEditorSettings.mJson );
        mData.Destroy();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Step()
    {
        const double elapsedTime = Time::ElapsedSinceStartup();
        IGame   & currentGame  = GetCurrentGame();
        EcsWorld& currentWorld = currentGame.GetWorld();
        Time    & currentTime  = currentWorld.GetSingleton<Time>();

        for( int i = 0; i < (int)mGames.size(); i++ )
        {
            IGame   & game  = *mGames[i];
            EcsWorld& world = game.GetWorld();

            // runs logic, renders ui
            Time& time = world.GetSingleton<Time>();
            while( elapsedTime > time.mLastLogicTime + time.mLogicDelta.ToDouble() )
            {
                time.mLastLogicTime += time.mLogicDelta.ToDouble();
                time.mFrameIndex++;

                const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
                const Fixed delta       = playState.mState == EditorPlayState::PLAYING ? time.mLogicDelta : 0;
                const Fixed scaledDelta = time.mLogicTimeScale * delta;

                world.GetSingleton<RenderDebug>().Clear();

                game.PreStep( scaledDelta );

                // physics & transforms
                world.Run<SIntegrateRigidbodies>( scaledDelta );
                world.Run<SDetectCollisions>( scaledDelta );
                world.Run<SMoveFollowTransforms>();

                // bounds
                world.Run<SUpdateBoundsFromFxBoxColliders>();
                world.Run<SUpdateBoundsFromFxSphereColliders>();
                world.Run<SUpdateBoundsFromModel>();
                world.Run<SUpdateBoundsFromTransform>();

                game.Step( scaledDelta );

                // animation
                world.Run<SUpdateAnimators>( scaledDelta );

                // ui
                world.Run<SUpdateUIText>();
                world.Run<SUpdateScalers>();
                world.Run<SAlignUI>();
                world.Run<SUpdateUILayouts>();
                world.Run<SHoverButtons>();
                world.Run<SHighlightButtons>();

                // gameplay
                world.Run<SUpdateExpirationTimes>( scaledDelta.ToFloat() );
                world.Run<SUpdateParticles>( scaledDelta.ToFloat() );
                world.Run<SEmitParticles>( scaledDelta.ToFloat() );
                world.Run<SGenerateParticles>( scaledDelta.ToFloat() );

                world.ApplyTransitions();

                if( &world == &currentWorld )
                {
                    const bool editorCameraActive = currentWorld.GetSingleton<Scene>().mMainCameraHandle == currentWorld.GetSingleton<EditorCamera>().mCameraHandle;
                    if( editorCameraActive )
                    {
                        EditorCamera::Update( currentWorld, currentTime.mLogicDelta );
                    }
                }

                currentWorld.GetSingleton<Mouse>().ClearSingleFrameEvents();
            }
        }

        mData.mOnLPPSynch.Emmit();

        GameViewWindow& currentGameViewWindow = currentWorld.GetSingleton<GameViewWindow>();

        // Render world
        const double deltaTime = elapsedTime - currentTime.mLastRenderTime;
        if( deltaTime > currentTime.mRenderDelta.ToDouble() )
        {
            ImGui::GetIO().DeltaTime    = float( deltaTime );
            currentTime.mLastRenderTime = elapsedTime;

            const bool editorCameraActive = currentWorld.GetSingleton<Scene>().mMainCameraHandle == currentWorld.GetSingleton<EditorCamera>().mCameraHandle;

            // don't draw if logic has not executed/cleared the debug buffers
            if( mLastLogicFrameRendered != currentTime.mFrameIndex )
            {
                mLastLogicFrameRendered = currentTime.mFrameIndex;

                // Update input
                Mouse::NextFrame( mData.mWindow.mWindow, currentGameViewWindow.mPosition, currentGameViewWindow.mSize );
                Input::Get().NewFrame();
                Input::Get().Manager().PullEvents();
                currentWorld.GetSingleton<Mouse>().UpdateData( mData.mWindow.mWindow );

                SCOPED_PROFILE( debug_draw );
                EditorMainMenuBar& mainMenuBar = currentWorld.GetSingleton<EditorMainMenuBar>();
                if( mainMenuBar.mShowWireframe ){ currentWorld.Run<SDrawDebugWireframe>(); }
                if( mainMenuBar.mShowNormals ){ currentWorld.Run<SDrawDebugNormals>(); }
                if( mainMenuBar.mShowAABB ){ currentWorld.Run<SDrawDebugBounds>(); }
                if( mainMenuBar.mShowBoundingSphere ){ currentWorld.Run<SDrawDebugBoundingSpheres>(); }
                if( mainMenuBar.mShowHull ){ currentWorld.Run<SDrawDebugHull>(); }
                if( mainMenuBar.mShowUiBounds ){ currentWorld.Run<SDrawDebugUiBounds>(); }
                if( mainMenuBar.mShowSkeletons ){ currentWorld.Run<SDrawSkeletons>(); }
                if( mainMenuBar.mShowLights )
                {
                    currentWorld.Run<SDrawDebugPointLights>();
                    currentWorld.Run<SDrawDebugDirectionalLights>();
                }
                EditorGrid::Draw( currentWorld );

                // only update the editor camera when we are using it
                if( editorCameraActive )
                {
                    EditorCamera::Update( currentWorld, currentTime.mLogicDelta );
                    currentWorld.GetSingleton<EditorSelection>().Update( currentGameViewWindow.mIsHovered );
                }

                // ImGui render
                ImGui::NewFrame();
                DrawEditorUI( currentWorld );
                currentGame.OnGui();
                ImGui::Render();
            }

            RenderWorld::Update( currentWorld );

            Time::RegisterFrameDrawn( currentTime, deltaTime );

            PlayerData::UpdateRenderWorld( mData.mRenderer, currentGame, currentGameViewWindow.mSize );
            mData.mRenderer.DrawFrame();

            const bool gameWindowHovered = currentWorld.GetSingleton<GameViewWindow>().mIsHovered;
            Cursor* currentCursor = !editorCameraActive && gameWindowHovered ? currentWorld.GetSingleton<Application>().mCurrentCursor : nullptr;

            PlayerData::MatchCursor( currentCursor, mData.mWindow );
            PlayerData::MatchFullscreenState( currentWorld.GetSingleton<RenderWorld>().mFullscreen, mData.mWindow );

            Profiler::Get().End();
            Profiler::Get().Begin();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Start( IGame& _game )
    {
        EcsWorld& world = _game.GetWorld();

        // saves the scene before playing
        Scene& scene = world.GetSingleton<Scene>();
        if( scene.mPath.empty() )
        {
            Debug::Warning() << "please save the scene before playing" << Debug::Endl();
            return;
        }

        EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            scene.Save();
            Debug::Highlight() << "game start" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
            _game.Start();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SceneRestoreState
    {
        SceneRestoreState( Scene& _scene ) : mScene( _scene ) {}
        void Save()
        {
            EcsWorld& world = *mScene.mWorld;

            // Saves the camera position for restoring it later
            const EcsEntity oldCameraID = world.GetEntity( mScene.mMainCameraHandle );
            mPrevCameraTransform = world.GetComponent<Transform>( oldCameraID );
            // save old selection
            SceneNode* prevSelectionNode = world.GetSingleton<EditorSelection>().GetSelectedSceneNode();
            mPrevSelectionHandle = prevSelectionNode != nullptr ? prevSelectionNode->mHandle : 0;
        }

        void Restore() const
        {
            EcsWorld& world = *mScene.mWorld;

            // restore camera transform
            const EcsEntity newCameraID = world.GetEntity( mScene.mMainCameraHandle );
            world.GetComponent<Transform>( newCameraID ) = mPrevCameraTransform;

            // restore selection
            if( mPrevSelectionHandle != 0 &&
                mScene.mNodes.find( mPrevSelectionHandle ) != mScene.mNodes.end() )
            {
                fan::SceneNode& node = world.GetComponent<fan::SceneNode>( world.GetEntity( mPrevSelectionHandle ) );
                world.GetSingleton<EditorSelection>().SetSelectedSceneNode( &node );
            }
        }

        Scene& mScene;
        EcsHandle mPrevSelectionHandle = 0;
        Transform mPrevCameraTransform;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Stop( IGame& _game )
    {
        EcsWorld& world = _game.GetWorld();

        EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING || playState.mState == EditorPlayState::PAUSED )
        {
            UseEditorCamera( world );

            Scene& scene = world.GetSingleton<Scene>();
            SceneRestoreState restoreState( scene );
            restoreState.Save();

            Debug::Highlight() << "game stopped" << Debug::Endl();
            playState.mState = EditorPlayState::STOPPED;
            _game.Stop();
            scene.LoadFrom( scene.mPath ); // reload the scene

            restoreState.Restore();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Pause( IGame& _game )
    {
        EcsWorld       & world     = _game.GetWorld();
        EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING )
        {
            Debug::Highlight() << "game paused" << Debug::Endl();
            playState.mState = EditorPlayState::PAUSED;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::Resume( IGame& _game )
    {
        EcsWorld       & world     = _game.GetWorld();
        EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PAUSED )
        {
            Debug::Highlight() << "game  resumed" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::OnSceneLoad( Scene& _scene )
    {
        EcsWorld& world = *_scene.mWorld;
        EditorCamera::CreateEditorCamera( world );

        if( &world == &GetCurrentGame().GetWorld() )
        {
            world.GetSingleton<EditorSelection>().Deselect();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::OnCycleCurrentGame()
    {
        OnSelect( ( mCurrentGame + 1 ) % ( mGames.size() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::OnStart()
    {
        IGame& game = GetCurrentGame();
        Start( game );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::OnSwitchPlayStop()
    {
        IGame                & game      = GetCurrentGame();
        const EditorPlayState& playState = game.GetWorld().GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            Start( game );
        }
        else
        {
            Stop( game );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::UseEditorCamera( EcsWorld& _world )
    {
        Scene       & scene        = _world.GetSingleton<Scene>();
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        scene.SetMainCamera( editorCamera.mCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::DrawEditorUI( EcsWorld& _world )
    {
        // Draw tool windows
        EditorMainMenuBar& mainMenuBar = _world.GetSingleton<EditorMainMenuBar>();
        EditorSettings   & settings    = _world.GetSingleton<EditorSettings>();

        // Draw main menu bar
        GuiSingletonInfo& mainMenuBarInfo = settings.GetSingletonInfo( EditorMainMenuBar::Info::sType );
        ( *mainMenuBarInfo.onGui )( _world, mainMenuBar );

        // Draw imgui demo window
        if( mainMenuBar.mShowImguiDemoWindow )
        {
            ImGui::ShowDemoWindow( &mainMenuBar.mShowImguiDemoWindow );
        }

        // Draw tool windows
        for( auto& pair : settings.mSingletonInfos )
        {
            GuiSingletonInfo& info    = pair.second;
            bool            & visible = settings.mData->mToolWindowsVisibility[pair.first];
            if( visible && info.mType == GuiSingletonInfo::Type::ToolWindow )
            {
                EcsSingleton& singleton = _world.GetSingleton( pair.first );
                if( ImGui::Begin( info.mEditorName.c_str(), &visible, info.mImGuiWindowFlags ) )
                {
                    ( *info.onGui )( _world, singleton );
                }
                ImGui::End();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::EcsIncludeEditor( EcsWorld& _world )
    {
        _world.AddSingletonType<EditorSettings>();
        _world.AddSingletonType<EditorPlayState>();
        _world.AddSingletonType<EditorCamera>();
        _world.AddSingletonType<EditorGrid>();
        _world.AddSingletonType<EditorSelection>();
        _world.AddSingletonType<EditorCopyPaste>();
        _world.AddSingletonType<EditorGizmos>();
        _world.AddSingletonType<EditorMainMenuBar>();
        _world.AddSingletonType<ConsoleWindow>();
        _world.AddSingletonType<EcsWindow>();
        _world.AddSingletonType<InspectorWindow>();
        _world.AddSingletonType<PreferencesWindow>();
        _world.AddSingletonType<ProfilerWindow>();
        _world.AddSingletonType<GameViewWindow>();
        _world.AddSingletonType<RenderWindow>();
        _world.AddSingletonType<SceneWindow>();
        _world.AddSingletonType<SingletonsWindow>();
        _world.AddSingletonType<TerrainWindow>();
        _world.AddSingletonType<UnitTestsWindow>();

        _world.AddTagType<TagEditorOnly>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorPlayer::OnOpen() { GetCurrentGame().GetWorld().GetSingleton<EditorMainMenuBar>().Open( GetCurrentGame().GetWorld() ); }
    void EditorPlayer::OnReload() { GetCurrentGame().GetWorld().GetSingleton<EditorMainMenuBar>().Reload( GetCurrentGame().GetWorld() ); }
    void EditorPlayer::OnSave() { GetCurrentGame().GetWorld().GetSingleton<EditorMainMenuBar>().Save( GetCurrentGame().GetWorld() ); }
    void EditorPlayer::OnCopy()
    {
        GetCurrentGame().GetWorld().GetSingleton<EditorCopyPaste>().OnCopy();
    }
    void EditorPlayer::OnPaste()
    {
        GetCurrentGame().GetWorld().GetSingleton<EditorCopyPaste>().OnPaste();
    }
    void EditorPlayer::OnDeleteSelection()
    {
        GetCurrentGame().GetWorld().GetSingleton<EditorSelection>().DeleteSelection();
    }
    void EditorPlayer::OnToogleTransformLock()
    {
        GetCurrentGame().GetWorld().GetSingleton<EditorSelection>().OnToogleTransformLock();
    }

    //==================================================================================================================================================================================================
    // sets which ecs world will be displayed in the editor
    //==================================================================================================================================================================================================
    void EditorPlayer::OnSelect( const int _index )
    {
        fanAssert( _index < (int)mGames.size() );

        mCurrentGame = _index;

        // Set all to headless except the current
        for( int i = 0; i < (int)mGames.size(); i++ )
        {
            IGame      & game        = *mGames[i];
            RenderWorld& renderWorld = game.GetWorld().GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( i != mCurrentGame );
        }

        GameViewWindow& gameViewWindow = GetCurrentGame().GetWorld().GetSingleton<GameViewWindow>();
        gameViewWindow.mCurrentGame = mCurrentGame;
    }

    //==================================================================================================================================================================================================
    // toogle between editor camera and game camera
    //==================================================================================================================================================================================================
    void EditorPlayer::OnToogleCamera()
    {
        IGame   & currentGame = GetCurrentGame();
        EcsWorld& world       = currentGame.GetWorld();

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
            world.GetSingleton<Application>().mOnEditorUseGameCamera.Emmit();
        }
        else
        {
            UseEditorCamera( world );
        }
    }

    //==================================================================================================================================================================================================
    // callback of the game view step button
    //==================================================================================================================================================================================================
    void EditorPlayer::OnStep()
    {
        IGame& game = GetCurrentGame();
        Time & time = game.GetWorld().GetSingleton<Time>();
        game.Step( time.mLogicDelta );
    }
}