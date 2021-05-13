
#include <engine/systems/fanUpdateBounds.hpp>
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/fanBits.hpp"

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
#include "editor/windows/fanTerrainWindow.hpp"

#include "editor/singletons/fanEditorMainMenuBar.hpp"
#include "editor/fanEditorProjectContainer.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EditorProjectContainer::EditorProjectContainer( LaunchSettings& _settings, const std::vector<IProject*>& _projects ) :
            IProjectContainer( AdaptSettings( _settings ) ),
            mProjects( _projects ),
            mLastLogicFrameRendered( 0 )
    {
        EditorSettingsData::LoadJsonFromDisk( mEditorSettings.mJson );
        EditorSettingsData::LoadSettingsFromJson( mEditorSettings );

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

        // Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, &mRenderer );
        manager.FindEvent( "play_pause" )->Connect( &EditorProjectContainer::OnSwitchPlayStop, this );
        manager.FindEvent( "copy" )->Connect( &EditorProjectContainer::OnCopy, this );
        manager.FindEvent( "paste" )->Connect( &EditorProjectContainer::OnPaste, this );
        manager.FindEvent( "show_ui" )->Connect( &EditorProjectContainer::OnToggleShowUI, this );
        manager.FindEvent( "toogle_camera" )->Connect( &EditorProjectContainer::OnToogleCamera, this );
        manager.FindEvent( "open_scene" )->Connect( &EditorProjectContainer::OnOpen, this );
        manager.FindEvent( "save_scene" )->Connect( &EditorProjectContainer::OnSave, this );
        manager.FindEvent( "reload_scene" )->Connect( &EditorProjectContainer::OnReload, this );
        manager.FindEvent( "toogle_world" )->Connect( &EditorProjectContainer::OnCycleCurrentProject, this );
        manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, &mRenderer );
        manager.FindEvent( "delete" )->Connect( &EditorProjectContainer::OnDeleteSelection, this );
        manager.FindEvent( "toogle_follow_transform_lock" )->Connect( &EditorProjectContainer::OnToogleTransformLock, this );

        // Loop over all worlds to initialize them
        for( int i = 0; i < (int)mProjects.size(); i++ )
        {
            IProject& project = *mProjects[i];
            EcsWorld& world   = project.mWorld;

            IProject::EcsIncludeEngine( world );
            IProject::EcsIncludePhysics( world );
            IProject::EcsIncludeRender3D( world );
            IProject::EcsIncludeRenderUI( world );
            EcsIncludeEditor( world );

            EditorSettings& editorSerializedValues = world.GetSingleton<EditorSettings>();
            editorSerializedValues.mData = &mEditorSettings;

            IProjectContainer::SetWorldResourcesPointers( world );

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.mIsHeadless = ( &project != &GetCurrentProject() );

            Scene          & scene     = world.GetSingleton<Scene>();
            EditorSelection& selection = world.GetSingleton<EditorSelection>();

            selection.ConnectCallbacks( scene );

            {
                EditorMainMenuBar& mainMenuBar = world.GetSingleton<EditorMainMenuBar>();
                mainMenuBar.mOnReloadShaders.Connect( &Renderer::ReloadShaders, &mRenderer );
                mainMenuBar.mOnReloadIcons.Connect( &Renderer::ReloadIcons, &mRenderer );
                mainMenuBar.mOnExit.Connect( &IProjectContainer::Exit, (IProjectContainer*)this );

                ProjectViewWindow& projectViewWindow = world.GetSingleton<ProjectViewWindow>();
                projectViewWindow.mOnSizeChanged.Connect( &Renderer::ResizeGame, &mRenderer );
                projectViewWindow.mOnPlay.Connect( &EditorProjectContainer::OnStart, this );
                projectViewWindow.mOnPause.Connect( &EditorProjectContainer::OnPause, this );
                projectViewWindow.mOnResume.Connect( &EditorProjectContainer::OnResume, this );
                projectViewWindow.mOnStop.Connect( &EditorProjectContainer::OnStop, this );
                projectViewWindow.mOnStep.Connect( &EditorProjectContainer::OnStep, this );
                projectViewWindow.mOnSelectProject.Connect( &EditorProjectContainer::OnSelect, this );

                SceneWindow& sceneWindow = world.GetSingleton<SceneWindow>();
                sceneWindow.onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );
                scene.mOnLoad.Connect( &SceneWindow::OnExpandHierarchy, &sceneWindow );
                scene.mOnLoad.Connect( &EditorProjectContainer::OnSceneLoad, this );
            }
            project.Init();
            world.PostInitSingletons( true );

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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    LaunchSettings& EditorProjectContainer::AdaptSettings( LaunchSettings& _settings )
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
    void EditorProjectContainer::Run()
    {
        Profiler::Get().Begin();
        while( mApplicationShouldExit == false && mWindow.IsOpen() == true )        // main loop
        {
            Step();
        }

        Debug::Log( "Exit application" );

        EditorSettingsData::SaveSettingsToJson( mEditorSettings );
        EditorSettingsData::SaveWindowSizeAndPosition( mEditorSettings.mJson, mRenderer.mWindow.GetSize(), mRenderer.mWindow.GetPosition() );
        EditorSettingsData::SaveJsonToDisk( mEditorSettings.mJson );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::Step()
    {
        const double elapsedTime = Time::ElapsedSinceStartup();
        IProject& currentProject = GetCurrentProject();
        EcsWorld& currentWorld   = currentProject.mWorld;
        Time    & currentTime    = currentWorld.GetSingleton<Time>();

        for( int i = 0; i < (int)mProjects.size(); i++ )
        {
            IProject& project = *mProjects[i];
            EcsWorld& world   = project.mWorld;

            // runs logic, renders ui
            Time& time = world.GetSingleton<Time>();
            while( elapsedTime > time.mLastLogicTime + time.mLogicDelta.ToDouble() )
            {
                time.mLastLogicTime += time.mLogicDelta.ToDouble();
                time.mFrameIndex++;

                world.GetSingleton<RenderDebug>().Clear();

                const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
                const Fixed delta = playState.mState == EditorPlayState::PLAYING ? time.mLogicDelta : 0;
                project.Step( delta );

                // bounds
                world.Run<SMoveFollowTransforms>();
                world.Run<SUpdateBoundsFromFxBoxColliders>();
                world.Run<SUpdateBoundsFromFxSphereColliders>();
                world.Run<SUpdateBoundsFromModel>();
                world.Run<SUpdateBoundsFromTransform>();

                world.ApplyTransitions();
            }
        }

        mOnLPPSynch.Emmit();

        ProjectViewWindow& currentProjectViewWindow = currentWorld.GetSingleton<ProjectViewWindow>();

        // Render world
        const double deltaTime = elapsedTime - currentTime.mLastRenderTime;
        if( deltaTime > currentTime.mRenderDelta.ToDouble() )
        {
            ImGui::GetIO().DeltaTime    = float( deltaTime );
            currentTime.mLastRenderTime = elapsedTime;

            // don't draw if logic has not executed/cleared the debug buffers
            if( mLastLogicFrameRendered != currentTime.mFrameIndex )
            {
                mLastLogicFrameRendered = currentTime.mFrameIndex;

                // Update input
                Mouse::NextFrame( mWindow.mWindow, currentProjectViewWindow.mPosition, currentProjectViewWindow.mSize );
                Input::Get().NewFrame();
                Input::Get().Manager().PullEvents();
                currentWorld.GetSingleton<Mouse>().UpdateData( mWindow.mWindow );

                SCOPED_PROFILE( debug_draw );
                EditorMainMenuBar& mainMenuBar = currentWorld.GetSingleton<EditorMainMenuBar>();
                if( mainMenuBar.mShowWireframe ){ currentWorld.Run<SDrawDebugWireframe>(); }
                if( mainMenuBar.mShowNormals ){ currentWorld.Run<SDrawDebugNormals>(); }
                if( mainMenuBar.mShowAABB ){ currentWorld.Run<SDrawDebugBounds>(); }
                if( mainMenuBar.mShowHull ){ currentWorld.Run<SDrawDebugHull>(); }
                if( mainMenuBar.mShowUiBounds ){ currentWorld.Run<SDrawDebugUiBounds>(); }
                if( mainMenuBar.mShowLights )
                {
                    currentWorld.Run<SDrawDebugPointLights>();
                    currentWorld.Run<SDrawDebugDirectionalLights>();
                }
                EditorGrid::Draw( currentWorld );

                EditorCamera& editorCamera = currentWorld.GetSingleton<EditorCamera>();
                Scene       & scene        = currentWorld.GetSingleton<Scene>();
                // only update the editor camera when we are using it
                if( scene.mMainCameraHandle == editorCamera.mCameraHandle )
                {
                    EditorCamera::Update( currentWorld, currentTime.mLogicDelta );
                }
                currentWorld.GetSingleton<EditorSelection>().Update( currentProjectViewWindow.mIsHovered );

                // ImGui render
                ImGui::NewFrame();
                DrawEditorUI( currentWorld );
                currentProject.OnGui();
                ImGui::Render();
            }

            currentProject.Render();

            Time::RegisterFrameDrawn( currentTime, deltaTime );

            UpdateRenderWorld( mRenderer, currentProject, currentProjectViewWindow.mSize );

            mRenderer.DrawFrame();

            Profiler::Get().End();
            Profiler::Get().Begin();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::Pause( IProject& _project )
    {
        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING )
        {
            Debug::Highlight() << _project.mName << ": paused" << Debug::Endl();
            playState.mState = EditorPlayState::PAUSED;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::Resume( IProject& _project )
    {
        EditorPlayState& playState = _project.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PAUSED )
        {
            Debug::Highlight() << _project.mName << ": resumed" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::OnSceneLoad( Scene& _scene )
    {
        EcsWorld& world = *_scene.mWorld;
        EditorCamera::CreateEditorCamera( world );

        if( &world == &GetCurrentProject().mWorld )
        {
            world.GetSingleton<EditorSelection>().Deselect();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::OnCycleCurrentProject()
    {
        OnSelect( ( mCurrentProject + 1 ) % ( mProjects.size() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::OnStart()
    {
        IProject& project = GetCurrentProject();
        Start( project );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::UseEditorCamera( EcsWorld& _world )
    {
        Scene       & scene        = _world.GetSingleton<Scene>();
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        scene.SetMainCamera( editorCamera.mCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::DrawEditorUI( EcsWorld& _world )
    {
        // Draw tool windows
        EditorGuiInfo    & guiInfos       = _world.GetSingleton<EditorGuiInfo>();
        EditorMainMenuBar& mainMenuBar    = _world.GetSingleton<EditorMainMenuBar>();
        EditorSettings   & editorSettings = _world.GetSingleton<EditorSettings>();

        // Draw main menu bar
        GuiSingletonInfo& mainMenuBarInfo = guiInfos.GetSingletonInfo( EditorMainMenuBar::Info::sType );
        ( *mainMenuBarInfo.onGui )( _world, mainMenuBar );

        // Draw imgui demo window
        if( mainMenuBar.mShowImguiDemoWindow )
        {
            ImGui::ShowDemoWindow( &mainMenuBar.mShowImguiDemoWindow );
        }

        // Draw tool windows
        for( auto& pair : guiInfos.mSingletonInfos )
        {
            GuiSingletonInfo& info    = pair.second;
            bool            & visible = editorSettings.mData->mToolWindowsVisibility[pair.first];
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
    void EditorProjectContainer::EcsIncludeEditor( EcsWorld& _world )
    {
        _world.AddSingletonType<EditorSettings>();
        _world.AddSingletonType<EditorGuiInfo>();
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
        _world.AddSingletonType<ProjectViewWindow>();
        _world.AddSingletonType<RenderWindow>();
        _world.AddSingletonType<SceneWindow>();
        _world.AddSingletonType<SingletonsWindow>();
        _world.AddSingletonType<TerrainWindow>();
        _world.AddSingletonType<UnitTestsWindow>();

        _world.AddTagType<TagEditorOnly>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorProjectContainer::OnOpen() { GetCurrentProject().mWorld.GetSingleton<EditorMainMenuBar>().Open( GetCurrentProject().mWorld ); }
    void EditorProjectContainer::OnReload() { GetCurrentProject().mWorld.GetSingleton<EditorMainMenuBar>().Reload( GetCurrentProject().mWorld ); }
    void EditorProjectContainer::OnSave() { GetCurrentProject().mWorld.GetSingleton<EditorMainMenuBar>().Save( GetCurrentProject().mWorld ); }
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

    //==================================================================================================================================================================================================
    // sets which ecs world will be displayed in the editor
    //==================================================================================================================================================================================================
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

        ProjectViewWindow& projectViewWindow = GetCurrentProject().mWorld.GetSingleton<ProjectViewWindow>();
        projectViewWindow.mCurrentProject = mCurrentProject;
    }

    //==================================================================================================================================================================================================
    // toogle between editor camera and project camera
    //==================================================================================================================================================================================================
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

    //==================================================================================================================================================================================================
    // callback of the project view step button
    //==================================================================================================================================================================================================
    void EditorProjectContainer::OnStep()
    {
        IProject& project = GetCurrentProject();
        Time    & time    = project.mWorld.GetSingleton<Time>();
        project.Step( time.mLogicDelta );
    }
}