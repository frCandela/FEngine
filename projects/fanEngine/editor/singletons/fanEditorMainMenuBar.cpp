#include "fanEditorMainMenuBar.hpp"

#include "core/fanDebug.hpp"
#include "core/fanPath.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "render/fanRenderGlobal.hpp"
#include "editor/gui/network/fanGuiTime.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        EditorMainMenuBar& mainMenuBar = static_cast<EditorMainMenuBar&>(_singleton);
        mainMenuBar.mOnReloadShaders.Clear();
        mainMenuBar.mOnReloadIcons.Clear();
        mainMenuBar.mOnExit.Clear();

        mainMenuBar.mShowImguiDemoWindow = false;
        mainMenuBar.mShowHull            = false;
        mainMenuBar.mShowAABB            = false;
        mainMenuBar.mShowBoundingSphere  = false;
        mainMenuBar.mShowWireframe       = false;
        mainMenuBar.mShowNormals         = false;
        mainMenuBar.mShowLights          = false;
        mainMenuBar.mShowUiBounds        = false;
        mainMenuBar.mShowSkeletons       = false;

        mainMenuBar.mOpenNewScenePopupLater  = false;
        mainMenuBar.mOpenLoadScenePopupLater = false;
        mainMenuBar.mOpenSaveScenePopupLater = false;

        mainMenuBar.mPathBuffer.clear();
    }

    //==================================================================================================================================================================================================
    // Draws the main menu bar and the editor windows
    //==================================================================================================================================================================================================
    void GuiEditorMainMenuBar::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        EditorMainMenuBar& mainMenuBar = static_cast<EditorMainMenuBar&>(_singleton);

        SCOPED_PROFILE( main_bar );

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->Pos );
        ImGui::SetNextWindowSize( viewport->Size );

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        // fullscreen
        {
            ImGui::SetNextWindowPos( viewport->Pos );
            ImGui::SetNextWindowSize( viewport->Size );
            ImGui::SetNextWindowViewport( viewport->ID );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
            window_flags |= ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        bool p_open = true;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::Begin( "DockSpace Demo", &p_open, window_flags );

        ImGui::PopStyleVar();
        ImGui::PopStyleVar( 2 );

        ImGui::DockSpace( ImGui::GetID( "MyDockSpace" ) );
        ImGui::End();

        // Draw main menu  bar
        if( ImGui::BeginMainMenuBar() )
        {
            // File
            if( ImGui::BeginMenu( "File" ) )
            {
                if( ImGui::MenuItem( "New" ) )
                {
                    EditorMainMenuBar::New( _world );
                }
                if( ImGui::MenuItem( "Open", "Ctrl+O" ) )
                {
                    EditorMainMenuBar::Open( _world );
                }
                if( ImGui::MenuItem( "Reload", "Ctrl+R" ) )
                {
                    EditorMainMenuBar::Reload( _world );
                }
                if( ImGui::MenuItem( "Save", "Ctrl+S" ) )
                {
                    EditorMainMenuBar::Save( _world );
                }
                if( ImGui::MenuItem( "Save as" ) )
                {
                    EditorMainMenuBar::SaveAs( _world );
                }

                ImGui::Separator();

                if( ImGui::MenuItem( "Reload shaders", "F11" ) )
                {
                    mainMenuBar.mOnReloadShaders.Emmit();
                }

                if( ImGui::MenuItem( "Reload icons", "F12" ) )
                {
                    mainMenuBar.mOnReloadIcons.Emmit();
                }

                if( ImGui::MenuItem( "Reload ecs infos" ) )
                {
                    _world.ReloadInfos();
                    EditorSettings& settings = _world.GetSingleton<EditorSettings>();
                    EditorSettings::InitComponentInfos( settings.mComponentInfos );
                    EditorSettings::InitSingletonInfos( settings.mSingletonInfos );
                }

                ImGui::Separator();

                if( ImGui::MenuItem( "Exit" ) )
                {
                    mainMenuBar.mOnExit.Emmit();
                }

                ImGui::EndMenu();
            }

            // View
            if( ImGui::BeginMenu( "Tools" ) )
            {
                ImGui::Icon( ImGui::Imgui16, { 16, 16 } );
                ImGui::SameLine();
                ImGui::MenuItem( "Imgui demo", nullptr, &mainMenuBar.mShowImguiDemoWindow );

                EditorSettings& settings = _world.GetSingleton<EditorSettings>();
                for( auto     & pair : settings.mSingletonInfos )
                {
                    const GuiSingletonInfo& info = pair.second;
                    if( info.mType == GuiSingletonInfo::Type::ToolWindow )
                    {
                        ImGui::Icon( info.mIcon, { 16, 16 } );
                        ImGui::SameLine();
                        const uint32_t singletonType = pair.first;
                        bool& visible = settings.mData->mToolWindowsVisibility[singletonType];
                        ImGui::MenuItem( info.mEditorName.c_str(), nullptr, &visible );
                    }
                }
                ImGui::EndMenu();
            }

            // Editor
            if( ImGui::BeginMenu( "View" ) )
            {
                if( ImGui::BeginMenu( "bounds" ) )
                {
                    if( ImGui::MenuItem( "hull", nullptr, &mainMenuBar.mShowHull ) ){}
                    if( ImGui::MenuItem( "aabb", nullptr, &mainMenuBar.mShowAABB ) ){}
                    if( ImGui::MenuItem( "sphere", nullptr, &mainMenuBar.mShowBoundingSphere ) ){}
                    if( ImGui::MenuItem( "ui rect", nullptr, &mainMenuBar.mShowUiBounds ) ){}
                    ImGui::EndMenu();
                }
                if( ImGui::BeginMenu( "geometry" ) )
                {
                    if( ImGui::MenuItem( "show wireframe", nullptr, &mainMenuBar.mShowWireframe ) ){}
                    if( ImGui::MenuItem( "show normals", nullptr, &mainMenuBar.mShowNormals ) ){}
                    if( ImGui::MenuItem( "show skeletons", nullptr, &mainMenuBar.mShowSkeletons ) ){}

                    ImGui::EndMenu();
                }
                if( ImGui::MenuItem( "lights", nullptr, &mainMenuBar.mShowLights ) ){}
                ImGui::EndMenu();
            }

            // Grid
            if( ImGui::BeginMenu( "Grid" ) )
            {
                ImGui::PushItemWidth( 150.f );

                const EditorSettings       & settings = _world.GetSingleton<EditorSettings>();
                const fan::GuiSingletonInfo& guiInfo  = settings.GetSingletonInfo( EditorGrid::Info::sType );
                EditorGrid                 & grid     = _world.GetSingleton<EditorGrid>();

                ( *guiInfo.onGui )( _world, grid );
                ImGui::PopItemWidth();

                ImGui::EndMenu();
            }

            // Framerate
            Time& time = _world.GetSingleton<Time>();
            ImGui::SameLine( ImGui::GetWindowWidth() - 60 );
            const int framerate = int( time.mAverageFrameTime == 0 ? 0 : 1 / time.mAverageFrameTime );
            if( ImGui::BeginMenu( ( std::to_string( framerate ) ).c_str(), false ) )
            {
                ImGui::EndMenu();
            }

            if( ImGui::IsItemClicked( 1 ) )
            {
                ImGui::OpenPopup( "main_menu_bar_set_fps" );
            }

            // Framerate set popup
            if( ImGui::BeginPopup( "main_menu_bar_set_fps" ) )
            {
                ImGui::PushItemWidth( 80.f );
                GuiTime::DrawTimeDeltaSetters( time );
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }
        }
        ImGui::EndMainMenuBar();


        // Open scene popup
        if( mainMenuBar.mOpenNewScenePopupLater == true )
        {
            mainMenuBar.mOpenNewScenePopupLater = false;
            ImGui::OpenPopup( "New scene" );
        }

        // Open scene popup
        if( mainMenuBar.mOpenLoadScenePopupLater == true )
        {
            mainMenuBar.mOpenLoadScenePopupLater = false;
            ImGui::OpenPopup( "Open scene" );
        }

        // Save scene popup
        if( mainMenuBar.mOpenSaveScenePopupLater == true )
        {
            mainMenuBar.mOpenSaveScenePopupLater = false;
            ImGui::OpenPopup( "Save scene" );
        }

        DrawModals( mainMenuBar, _world.GetSingleton<Scene>() );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiEditorMainMenuBar::DrawModals( EditorMainMenuBar& _mainMenuBar, Scene& _scene )
    {
        // New scene
        if( ImGui::FanSaveFileModal( "New scene", RenderGlobal::sSceneExtensions, _mainMenuBar.mPathBuffer ) )
        {
            _scene.New();
            _scene.mPath = _mainMenuBar.mPathBuffer;
        }

        // Open scenes
        if( ImGui::FanLoadFileModal( "Open scene", RenderGlobal::sSceneExtensions, _mainMenuBar.mPathBuffer ) )
        {
            Debug::Log() << "loading scene: " << _mainMenuBar.mPathBuffer << Debug::Endl();
            _scene.LoadFrom( _mainMenuBar.mPathBuffer );
        }

        // Save scene
        if( ImGui::FanSaveFileModal( "Save scene", RenderGlobal::sSceneExtensions, _mainMenuBar.mPathBuffer ) )
        {
            _scene.mPath = _mainMenuBar.mPathBuffer;
            Debug::Log() << "saving scene: " << _scene.mPath << Debug::Endl();
            _scene.Save();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::New( EcsWorld& _world )
    {
        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
        if( playState.mState != EditorPlayState::STOPPED )
        {
            Debug::Warning() << "creating scenes is disabled in play mode" << Debug::Endl();
            return;
        }

        EditorMainMenuBar& mainMenuBar = _world.GetSingleton<EditorMainMenuBar>();
        mainMenuBar.mPathBuffer             = Path::Normalize( RenderGlobal::sScenesPath );
        mainMenuBar.mOpenNewScenePopupLater = true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::Open( EcsWorld& _world )
    {
        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
        if( playState.mState != EditorPlayState::STOPPED )
        {
            Debug::Warning() << "loading scenes is disabled in play mode" << Debug::Endl();
            return;
        }

        EditorMainMenuBar& mainMenuBar = _world.GetSingleton<EditorMainMenuBar>();
        mainMenuBar.mPathBuffer              = Path::Normalize( RenderGlobal::sScenesPath );
        mainMenuBar.mOpenLoadScenePopupLater = true;
    }

    //==================================================================================================================================================================================================
    // reload the scene
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::Reload( EcsWorld& _world )
    {
        Scene& scene = _world.GetSingleton<Scene>();

        if( scene.mPath.empty() )
        {
            Debug::Warning( "you cannot reload a scene that is not saved." );
            return;
        }

        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            EditorSelection& editorSelection = _world.GetSingleton<EditorSelection>();

            // save old camera transform
            const EcsEntity oldCameraID        = _world.GetEntity( scene.mMainCameraHandle );
            Transform       oldCameraTransform = _world.GetComponent<Transform>( oldCameraID );

            // save old selection
            SceneNode* prevSelectionNode = editorSelection.GetSelectedSceneNode();
            const EcsHandle prevSelectionHandle = prevSelectionNode != nullptr ? prevSelectionNode->mHandle : 0;

            Debug::Log() << "loading scene: " << scene.mPath << Debug::Endl();
            scene.LoadFrom( scene.mPath );

            // restore camera
            const EcsEntity newCameraID = _world.GetEntity( scene.mMainCameraHandle );
            _world.GetComponent<Transform>( newCameraID ) = oldCameraTransform;

            // restore selection
            if( prevSelectionHandle != 0 && scene.mNodes.find( prevSelectionHandle ) != scene.mNodes.end() )
            {
                EcsEntity entity = _world.GetEntity( prevSelectionHandle );
                fan::SceneNode& node = _world.GetComponent<fan::SceneNode>( entity );
                editorSelection.SetSelectedSceneNode( &node );
            }
        }
        else
        {
            Debug::Warning() << "reload is disabled in play mode" << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::Save( EcsWorld& _world )
    {
        Scene& scene = _world.GetSingleton<Scene>();

        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
        if( playState.mState != EditorPlayState::STOPPED )
        {
            Debug::Warning() << "saving is disabled in play mode" << Debug::Endl();
            return;
        }

        if( !scene.mPath.empty() )
        {
            scene.Save();
        }
        else
        {
            SaveAs( _world );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorMainMenuBar::SaveAs( EcsWorld& _world )
    {
        EditorMainMenuBar& mainMenuBar = _world.GetSingleton<EditorMainMenuBar>();
        mainMenuBar.mPathBuffer              = Path::Normalize( RenderGlobal::sScenesPath );
        mainMenuBar.mOpenSaveScenePopupLater = true;
    }
}