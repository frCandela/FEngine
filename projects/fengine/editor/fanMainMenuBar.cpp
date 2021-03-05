#include "editor/fanMainMenuBar.hpp"

#include "core/fanDebug.hpp"
#include "core/fanFileSystem.hpp"
#include "core/memory/fanSerializedValues.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderGlobal.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanTransform.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	MainMenuBar::MainMenuBar()
		: mSceneExtensionFilter( RenderGlobal::sSceneExtensions )
	{
		SerializedValues::Get().GetBool( "show_imguidemo", mShowImguiDemoWindow );
	}

	//========================================================================================================
	//========================================================================================================
	MainMenuBar::~MainMenuBar()
	{
		SerializedValues::Get().SetBool( "show_imguidemo", mShowImguiDemoWindow );

		for ( int windowIndex = 0; windowIndex < (int)mEditorWindows.size(); windowIndex++ )
		{
			delete mEditorWindows[ windowIndex ];
		}
	}

	//========================================================================================================
	// All editor windows are drawn & deleted with the main menubar
	//========================================================================================================
	void MainMenuBar::SetWindows( std::vector< EditorWindow* > _editorWindows )
	{
        mEditorWindows = _editorWindows;
	}

	//========================================================================================================
	// Draws the main menu bar and the editor windows
	//========================================================================================================
	void MainMenuBar::Draw( EcsWorld& _world )
	{
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

		// Draw editor windows
		for ( int windowIndex = 0; windowIndex < (int)mEditorWindows.size(); windowIndex++ )
		{
			mEditorWindows[ windowIndex ]->Draw( _world );
		}

		// Draw imgui demo
		if ( mShowImguiDemoWindow )
		{
			ImGui::ShowDemoWindow( &mShowImguiDemoWindow );
		}

		// Draw main menu  bar
		if ( ImGui::BeginMainMenuBar() )
		{
			// File
			if ( ImGui::BeginMenu( "File" ) )
			{
				if ( ImGui::MenuItem( "New" ) )
				{
					New( _world );
				}
				if ( ImGui::MenuItem( "Open", "Ctrl+O" ) )
				{
					Open( _world );
				}
				if ( ImGui::MenuItem( "Reload", "Ctrl+R" ) )
				{
					Reload( _world );
				}
				if ( ImGui::MenuItem( "Save", "Ctrl+S" ) )
				{
					Save( _world );
				}
				if ( ImGui::MenuItem( "Save as" ) )
				{
					SaveAs();
				}

				ImGui::Separator();

				if ( ImGui::MenuItem( "Reload shaders", "F11" ) )
				{
					mOnReloadShaders.Emmit();
				}

				if ( ImGui::MenuItem( "Reload icons", "F12" ) )
				{
					mOnReloadIcons.Emmit();
				}

                if ( ImGui::MenuItem( "Reload ecs infos" ) )
                {
                    _world.ReloadInfos();
                }

				ImGui::Separator();

				if ( ImGui::MenuItem( "Exit" ) )
				{
					mOnExit.Emmit();
				}

				ImGui::EndMenu();
			}

			// View
			if ( ImGui::BeginMenu( "Tools" ) )
			{
				ImGui::Icon( ImGui::Imgui16, { 16, 16 } ); ImGui::SameLine();
				ImGui::MenuItem( "Imgui demo", nullptr, &mShowImguiDemoWindow );

				for ( size_t windowIndex = 0; windowIndex < mEditorWindows.size(); windowIndex++ )
				{
					EditorWindow* window = mEditorWindows[ windowIndex ];
					ImGui::Icon( window->GetIconType(), { 16,16 } ); ImGui::SameLine();
					bool showWindow = window->IsVisible();
					if ( ImGui::MenuItem( window->GetName().c_str(), nullptr, &showWindow ) )
					{
						window->SetVisible( showWindow );
					}
				}
				ImGui::EndMenu();
			}

			// Editor
			if ( ImGui::BeginMenu( "View" ) )
			{
				if ( ImGui::MenuItem( "show hull", nullptr, &mShowHull ) ) {}
				if ( ImGui::MenuItem( "show AABB", nullptr, &mShowAABB ) ) {}
				if ( ImGui::MenuItem( "show wireframe", nullptr, &mShowWireframe ) ) {}
				if ( ImGui::MenuItem( "show normals", nullptr, &mShowNormals ) ) {}
				if( ImGui::MenuItem(  "show lights", nullptr, &mShowLights ) ) {}
                if( ImGui::MenuItem(  "show ui bounds", nullptr, &mShowUiBounds ) ) {}
				
				ImGui::EndMenu();
			}

			// Grid
			if ( ImGui::BeginMenu( "Grid" ) )
			{
				ImGui::PushItemWidth( 150.f );

                const EditorGuiInfo& gui = _world.GetSingleton<EditorGuiInfo>();
                const fan::GuiSingletonInfo& guiInfo = gui.GetSingletonInfo( EditorGrid::Info::sType );
                EditorGrid& grid = _world.GetSingleton<EditorGrid>();

                ( *guiInfo.onGui )( _world, grid );
				ImGui::PopItemWidth();

				ImGui::EndMenu();
			}

			// Framerate
			ImGui::SameLine( ImGui::GetWindowWidth() - 60 );
			if ( ImGui::BeginMenu( std::to_string( Time::sRealFramerateLastSecond ).c_str(), false ) )
			{
			    ImGui::EndMenu();
			}
			ImGui::FanToolTip( " Framerate. (Right click to set)" );

			if ( ImGui::IsItemClicked( 1 ) )
			{
				ImGui::OpenPopup( "main_menu_bar_set_fps" );
			}

			// Framerate set popup
			if ( ImGui::BeginPopup( "main_menu_bar_set_fps" ) )
			{
				Time& time = _world.GetSingleton<Time>();

				ImGui::PushItemWidth( 80.f );
				float maxFps = 1.f / Time::sRenderDelta;
				if ( ImGui::DragFloat( "fps", &maxFps, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					Time::sRenderDelta = maxFps < 1.f ? 1.f : 1.f / maxFps ;
				}
				float maxLogicFrequency = 1.f / time.mLogicDelta;
				if ( ImGui::DragFloat( "logic frequency", &maxLogicFrequency, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					time.mLogicDelta = maxLogicFrequency < 1.f ? 1.f : 1.f / maxLogicFrequency;
				}
				float maxPhysicsFrequency = 1.f / Time::sPhysicsDelta;
				if ( ImGui::DragFloat( "physics frequency", &maxPhysicsFrequency, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					Time::sPhysicsDelta = maxPhysicsFrequency < 1.f ? 1.f : 1.f / maxPhysicsFrequency;
				}
				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}

		} ImGui::EndMainMenuBar();


		// Open scene popup
		if ( mOpenNewScenePopupLater == true )
		{
            mOpenNewScenePopupLater = false;
			ImGui::OpenPopup( "New scene" );
		}

		// Open scene popup
		if ( mOpenLoadScenePopupLater == true )
		{
            mOpenLoadScenePopupLater = false;
			ImGui::OpenPopup( "Open scene" );
		}

		// Save scene popup
		if ( mOpenSaveScenePopupLater == true )
		{
            mOpenSaveScenePopupLater = false;
			ImGui::OpenPopup( "Save scene" );
		}

		DrawModals( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void MainMenuBar::DrawModals( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();

		// New scene
		if ( ImGui::FanSaveFileModal( "New scene", RenderGlobal::sSceneExtensions, mPathBuffer ) )
		{		
			scene.New();
			scene.mPath = mPathBuffer.string();
		}

		// Open scenes
		if ( ImGui::FanLoadFileModal( "Open scene", mSceneExtensionFilter, mPathBuffer ) )
		{
			Debug::Log() << "loading scene: " << mPathBuffer.string() << Debug::Endl();
			scene.LoadFrom( mPathBuffer.string() );
		}

		// Save scene
		if ( ImGui::FanSaveFileModal( "Save scene", RenderGlobal::sSceneExtensions, mPathBuffer ) )
		{
			scene.mPath = mPathBuffer.string();
			Debug::Get() << Debug::Severity::log << "saving scene: " << scene.mPath << Debug::Endl();
			scene.Save();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void MainMenuBar::New( EcsWorld& _world )
	{
        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
		if( playState.mState != EditorPlayState::STOPPED )
		{
			Debug::Warning() << "creating scenes is disabled in play mode" << Debug::Endl();
			return;
		}

        mPathBuffer             = FileSystem::NormalizePath( RenderGlobal::sScenesPath );
        mOpenNewScenePopupLater = true;
	}

	//========================================================================================================
	//========================================================================================================
	void MainMenuBar::Open( EcsWorld& _world )
	{
        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
		if( playState.mState != EditorPlayState::STOPPED )
		{
			Debug::Warning() << "loading scenes is disabled in play mode" << Debug::Endl();
			return;
		}

        mPathBuffer              = FileSystem::NormalizePath( RenderGlobal::sScenesPath );
        mOpenLoadScenePopupLater = true;
	}

	//========================================================================================================
	// reload the scene
	//========================================================================================================
	void MainMenuBar::Reload( EcsWorld& _world )
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
			const EcsEntity oldCameraID = _world.GetEntity( scene.mMainCameraHandle );
			btTransform oldCameraTransform = _world.GetComponent<Transform>( oldCameraID ).mTransform;

			// save old selection
			SceneNode* prevSelectionNode = editorSelection.GetSelectedSceneNode();
            const EcsHandle prevSelectionHandle = prevSelectionNode != nullptr ?
                    prevSelectionNode->mHandle :
                    0;

			Debug::Get() << Debug::Severity::log << "loading scene: " << scene.mPath << Debug::Endl();
			scene.LoadFrom( scene.mPath );

			// restore camera
			const EcsEntity newCameraID = _world.GetEntity( scene.mMainCameraHandle );
			_world.GetComponent<Transform>( newCameraID ).mTransform = oldCameraTransform;

			// restore selection
			if( prevSelectionHandle != 0 && scene.mNodes.find( prevSelectionHandle) != scene.mNodes.end() )
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

	//========================================================================================================
	//========================================================================================================
	void MainMenuBar::Save( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton<Scene>();

        const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
		if( playState.mState != EditorPlayState::STOPPED )
		{
			Debug::Warning() << "saving is disabled in play mode" << Debug::Endl();
			return;
		}

		if ( ! scene.mPath.empty() )
		{
			Debug::Get() << Debug::Severity::log << "saving scene: " << scene.mPath << Debug::Endl();
			scene.Save();
		}
		else
		{
			SaveAs();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void MainMenuBar::SaveAs()
	{
        mPathBuffer              = FileSystem::NormalizePath( RenderGlobal::sScenesPath );
        mOpenSaveScenePopupLater = true;
	}
}