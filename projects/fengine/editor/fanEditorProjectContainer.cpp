
#include <engine/systems/fanUpdateBounds.hpp>
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"

#include "network/singletons/fanTime.hpp"

#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/project/fanIProject.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanDrawDebug.hpp"

#include "editor/windows/fanPreferencesWindow.hpp"
#include "editor/windows/fanUnitsTestsWindow.hpp"
#include "editor/windows/fanSingletonsWindow.hpp"
#include "editor/windows/fanInspectorWindow.hpp"
#include "editor/windows/fanProfilerWindow.hpp"
#include "editor/windows/fanProjectViewWindow.hpp"
#include "editor/windows/fanConsoleWindow.hpp"
#include "editor/windows/fanRenderWindow.hpp"
#include "editor/windows/fanSceneWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/fanEditorProjectContainer.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    EditorProjectContainer::EditorProjectContainer( LaunchSettings& _settings, const std::vector<IProject*>& _projects ) :
            IProjectContainer( AdaptSettings( _settings) ),
            mProjects( _projects )
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

        // Initialize editor components
        mMainMenuBar       = new MainMenuBar();
        mProjectViewWindow = new ProjectViewWindow( /*_settings.launchMode*/ ); // @todo repair this
        SceneWindow& sceneWindow = *new SceneWindow();
        mMainMenuBar->SetWindows( {
                                          mProjectViewWindow,
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
        mMainMenuBar->mOnExit.Connect( &IProjectContainer::Exit, (IProjectContainer*)this );

        // Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, &mRenderer );
        manager.FindEvent( "play_pause" )->Connect( &EditorProjectContainer::OnSwitchPlayStop, this );
        manager.FindEvent( "copy" )->Connect( &EditorProjectContainer::OnCopy, this );
        manager.FindEvent( "paste" )->Connect( &EditorProjectContainer::OnPaste, this );
        manager.FindEvent( "show_ui" )->Connect( &EditorProjectContainer::OnToogleShowUI, this );
        manager.FindEvent( "toogle_camera" )->Connect( &EditorProjectContainer::OnToogleCamera, this );
        manager.FindEvent( "open_scene" )->Connect( &EditorProjectContainer::OnOpen, this );
        manager.FindEvent( "save_scene" )->Connect( &EditorProjectContainer::OnSave, this );
        manager.FindEvent( "reload_scene" )->Connect( &EditorProjectContainer::OnReload, this );
        manager.FindEvent( "toogle_world" )->Connect( &EditorProjectContainer::OnCycleCurrentProject, this );
        manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, &mRenderer );
        manager.FindEvent( "delete" )->Connect( &EditorProjectContainer::OnDeleteSelection, this );
        manager.FindEvent( "toogle_follow_transform_lock" )->Connect( &EditorProjectContainer::OnToogleTransformLock, this );

        mProjectViewWindow->mOnSizeChanged.Connect( &Renderer::ResizeGame, &mRenderer );
        mProjectViewWindow->mOnPlay.Connect( &EditorProjectContainer::OnStart, this );
        mProjectViewWindow->mOnPause.Connect( &EditorProjectContainer::OnPause, this );
        mProjectViewWindow->mOnResume.Connect( &EditorProjectContainer::OnResume, this );
        mProjectViewWindow->mOnStop.Connect( &EditorProjectContainer::OnStop, this );
        mProjectViewWindow->mOnStep.Connect( &EditorProjectContainer::OnStep, this );
        mProjectViewWindow->mOnSelectProject.Connect( &EditorProjectContainer::OnSelect, this );

        // Loop over all worlds to initialize them
        for( int i = 0; i < (int)mProjects.size(); i++ )
        {
            IProject& project = *mProjects[i];
            EcsWorld& world   = project.mWorld;

            world.AddSingletonType<EditorGuiInfo>();
            project.Init();
            world.AddSingletonType<EditorPlayState>();
            world.AddSingletonType<EditorCamera>();
            world.AddSingletonType<EditorGrid>();
            world.AddSingletonType<EditorSelection>();
            world.AddSingletonType<EditorCopyPaste>();
            world.AddSingletonType<EditorGizmos>();
            world.AddTagType<TagEditorOnly>();

            InitWorld( world );

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( &project != &GetCurrentProject() );

            Scene          & scene     = world.GetSingleton<Scene>();
            EditorSelection& selection = world.GetSingleton<EditorSelection>();

            selection.ConnectCallbacks( scene );
            sceneWindow.onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );

            scene.mOnLoad.Connect( &SceneWindow::OnExpandHierarchy, &sceneWindow );
            scene.mOnLoad.Connect( &EditorProjectContainer::OnSceneLoad, this );

            // load scene
            scene.New();
            if( !_settings.mLoadScene.empty() )
            {
                scene.LoadFrom( _settings.mLoadScene );

                // auto play the scene
                if( _settings.mAutoPlay )
                {
                    Start( project );
                }
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    LaunchSettings& EditorProjectContainer::AdaptSettings( LaunchSettings& _settings )
    {
        if( ! _settings.mForceWindowDimensions )
        {
            SerializedValues::LoadWindowPosition( _settings.mWindow_position );
            SerializedValues::LoadWindowSize( _settings.mWindow_size );
        }
        _settings.mIconPath     = RenderGlobal::sEditorIcon;
        _settings.mLaunchEditor = true;
        return _settings;
    }

    //========================================================================================================
    //========================================================================================================
    EditorProjectContainer::~EditorProjectContainer()
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
    void EditorProjectContainer::Run()
    {
        // initializes timers
        mLastRenderTime = Time::ElapsedSinceStartup();
        for( IProject* project : mProjects )
        {
            Time& time = project->mWorld.GetSingleton<Time>();
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
    void EditorProjectContainer::Step()
    {
        const double currentTime       = Time::ElapsedSinceStartup();
        const bool   renderIsThisFrame = currentTime > mLastRenderTime + Time::sRenderDelta;
        const Time& currentWorldTime = GetCurrentProject().mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      currentWorldTime.mLastLogicTime + currentWorldTime.mLogicDelta;

        // Update all worlds
        for( int i = 0; i < (int)mProjects.size(); i++ )
        {
            IProject& project = *mProjects[i];
            EcsWorld& world   = project.mWorld;
            const bool isCurrentWorld = ( &project == &GetCurrentProject() );

            // runs logic, renders ui
            Time& time = world.GetSingleton<Time>();
            while( currentTime > time.mLastLogicTime + time.mLogicDelta )
            {
                world.GetSingleton<RenderDebug>().Clear();

                if( isCurrentWorld )
                {
                    // Update input
                    ImGui::GetIO().DeltaTime = time.mLogicDelta;
                    const glm::vec2 viewPosition = mProjectViewWindow->GetPosition();
                    const glm::vec2 viewSize     = mProjectViewWindow->GetSize();
                    //todo mGameViewWindow->IsHovered()
                    Mouse::NextFrame( mWindow.mWindow, viewPosition, viewSize );
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
                const float delta = playState.mState == EditorPlayState::PLAYING ? time.mLogicDelta : 0.f;
                project.Step( delta );

                // bounds
                world.Run<SUpdateBoundsFromRigidbody>( delta );
                world.Run<SUpdateBoundsFromModel>();
                world.Run<SUpdateBoundsFromTransform>();
                world.Run<SUpdateBoundsFromFxSphereColliders>();
                world.Run<SUpdateBoundsFromFxBoxColliders>();

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
                    world.GetSingleton<EditorSelection>().Update( mProjectViewWindow->IsHovered() );

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
                        EditorGrid::Draw( GetCurrentProject().mWorld );

                        // ImGui render
                        {
                            SCOPED_PROFILE( ImGui_render );
                            ImGui::NewFrame();
                            mMainMenuBar->Draw( project.mWorld );
                            GetCurrentProject().OnGui();
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


            UpdateRenderWorld( mRenderer, GetCurrentProject(), mProjectViewWindow->GetSize() );

            mRenderer.DrawFrame();
            Profiler::Get().End();
            Profiler::Get().Begin();
        }

        // sleep for the rest of the frame
        if( mLaunchSettings.mMainLoopSleep )
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
    void EditorProjectContainer::Start( IProject& _project )
    {
        // saves the scene before playing
        Scene& scene = _project.mWorld.GetSingleton<Scene>();
        if( scene.mPath.empty() )
        {
            Debug::Warning() << "please save the scene before playing" << Debug::Endl();
            return;
        }

        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            scene.Save();
            Debug::Highlight() << _project.mName << ": start" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
            _project.Start();
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
    void EditorProjectContainer::Stop( IProject& _project )
    {
        EcsWorld& world = _project.mWorld;

        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING || playState.mState == EditorPlayState::PAUSED )
        {
            UseEditorCamera( world );

            Scene& scene = world.GetSingleton<Scene>();
            SceneRestoreState restoreState( scene );
            restoreState.Save();

            Debug::Highlight() << _project.mName << ": stopped" << Debug::Endl();
            playState.mState = EditorPlayState::STOPPED;
            _project.Stop();
            scene.LoadFrom( scene.mPath ); // reload the scene

            restoreState.Restore();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::Pause( IProject& _project )
    {
        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING )
        {
            Debug::Highlight() << _project.mName << ": paused" << Debug::Endl();
            playState.mState = EditorPlayState::PAUSED;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::Resume( IProject& _project )
    {
        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PAUSED )
        {
            Debug::Highlight() << _project.mName << ": resumed" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::OnSceneLoad( Scene& _scene )
    {
        EcsWorld& world = *_scene.mWorld;
        EditorCamera::CreateEditorCamera( world );

        if( &world == &GetCurrentProject().mWorld )
        {
            world.GetSingleton<EditorSelection>().Deselect();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::OnCycleCurrentProject()
    {
        OnSelect( ( mCurrentProject + 1 ) % ( mProjects.size() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::OnStart()
    {
        IProject& project = GetCurrentProject();
        Start( project );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::OnSwitchPlayStop()
    {
        IProject             & project   = GetCurrentProject();
        const EditorPlayState& playState = project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            Start( project );
        }
        else
        {
            Stop( project );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::UseEditorCamera( EcsWorld& _world )
    {
        Scene       & scene        = _world.GetSingleton<Scene>();
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        scene.SetMainCamera( editorCamera.mCameraHandle );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorProjectContainer::OnOpen() { mMainMenuBar->Open( GetCurrentProject().mWorld ); }
    void EditorProjectContainer::OnReload() { mMainMenuBar->Reload( GetCurrentProject().mWorld ); }
    void EditorProjectContainer::OnSave() { mMainMenuBar->Save( GetCurrentProject().mWorld ); }
    void EditorProjectContainer::OnCopy()
    {
        GetCurrentProject().mWorld.GetSingleton<EditorCopyPaste>().OnCopy();
    }
    void EditorProjectContainer::OnPaste()
    {
        GetCurrentProject().mWorld.GetSingleton<EditorCopyPaste>().OnPaste();
    }
    void EditorProjectContainer::OnDeleteSelection()
    {
        GetCurrentProject().mWorld.GetSingleton<EditorSelection>().DeleteSelection();
    }
    void EditorProjectContainer::OnToogleTransformLock()
    {
        GetCurrentProject().mWorld.GetSingleton<EditorSelection>().OnToogleTransformLock();
    }

    //========================================================================================================
    // sets which ecs world will be displayed in the editor
    //========================================================================================================
    void EditorProjectContainer::OnSelect( const int _index )
    {
        fanAssert( _index < (int)mProjects.size() );

        mCurrentProject = _index;

        // Set all to headless except the current
        for( int i = 0; i < (int)mProjects.size(); i++ )
        {
            IProject   & project     = *mProjects[i];
            RenderWorld& renderWorld = project.mWorld.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( i != mCurrentProject );
        }

        mProjectViewWindow->SetCurrentProject( mCurrentProject );
    }

    //========================================================================================================
    // toogle between editor camera and project camera
    //========================================================================================================
    void EditorProjectContainer::OnToogleCamera()
    {
        IProject& currentProject = GetCurrentProject();
        EcsWorld& world          = currentProject.mWorld;

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
            world.GetSingleton<Scene>().mOnEditorUseProjectCamera.Emmit();
        }
        else
        {
            UseEditorCamera( world );
        }
    }

    //========================================================================================================
    // callback of the project view step button
    //========================================================================================================
    void EditorProjectContainer::OnStep()
    {
        IProject& project = GetCurrentProject();
        Time    & time    = project.mWorld.GetSingleton<Time>();
        project.Step( time.mLogicDelta );
    }
}