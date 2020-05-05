#include "editor/fanMainMenuBar.hpp"

#include "core/fanDebug.hpp"
#include "editor/singletonComponents/fanEditorGrid.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "editor/fanEditorSelection.hpp"
#include "editor/fanModals.hpp"
#include "core/fanSerializedValues.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar( EcsWorld& _world, EditorSelection& _editorSelection )
		: m_editorSelection( _editorSelection )
		, m_showImguiDemoWindow( true )
		, m_showAABB( false )
		, m_showHull( false )
		, m_showWireframe( false )
		, m_showNormals( false )
		, m_showLights( false )
		, m_sceneExtensionFilter( RenderGlobal::s_sceneExtensions )
		, m_world( &_world )
	{

		SerializedValues::Get().GetBool( "show_imguidemo", m_showImguiDemoWindow );

		Input::Get().Manager().FindEvent( "open_scene" )->Connect( &MainMenuBar::Open, this );
		Input::Get().Manager().FindEvent( "save_scene" )->Connect( &MainMenuBar::Save, this );
		Input::Get().Manager().FindEvent( "reload_scene" )->Connect( &MainMenuBar::Reload, this );
		Input::Get().Manager().FindEvent( "reload_icons" )->Connect( &Signal<>::Emmit, &this->onReloadIcons );
	}

	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::~MainMenuBar()
	{
		SerializedValues::Get().SetBool( "show_imguidemo", m_showImguiDemoWindow );
		SerializedValues::Get().SetBool( "editor_grid_show", m_editorGrid->isVisible );
		SerializedValues::Get().SetFloat( "editor_grid_spacing", m_editorGrid->spacing );
		SerializedValues::Get().SetInt( "editor_grid_linesCount", m_editorGrid->linesCount );
		SerializedValues::Get().SetColor( "editor_grid_color", m_editorGrid->color );
		SerializedValues::Get().SetVec3( "editor_grid_offset", m_editorGrid->offset );

		for ( int windowIndex = 0; windowIndex < m_editorWindows.size(); windowIndex++ )
		{
			delete m_editorWindows[ windowIndex ];
		}


	}

	//================================================================================================================================
	// All editor windows are drawn & deleted with the main menubar
	//================================================================================================================================
	void MainMenuBar::SetWindows( std::vector< EditorWindow* > _editorWindows )
	{
		m_editorWindows = _editorWindows;
	}

	//================================================================================================================================
	// Draws the main menu bar and the editor windows
	//================================================================================================================================
	void MainMenuBar::Draw()
	{
		SCOPED_PROFILE( main_bar );


		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos( viewport->Pos );
		ImGui::SetNextWindowSize( viewport->Size );

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		//fillscreen
		{
			ImGui::SetNextWindowPos( viewport->Pos );
			ImGui::SetNextWindowSize( viewport->Size );
			ImGui::SetNextWindowViewport( viewport->ID );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
		for ( int windowIndex = 0; windowIndex < m_editorWindows.size(); windowIndex++ )
		{
			m_editorWindows[ windowIndex ]->Draw();
		}

		// Draw imgui demo
		if ( m_showImguiDemoWindow )
		{
			ImGui::ShowDemoWindow( &m_showImguiDemoWindow );
		}

		// Draw main menu  bar
		if ( ImGui::BeginMainMenuBar() )
		{
			// File
			if ( ImGui::BeginMenu( "File" ) )
			{
				if ( ImGui::MenuItem( "New" ) )
				{
					New();
				}
				if ( ImGui::MenuItem( "Open", "Ctrl+O" ) )
				{
					Open();
				}
				if ( ImGui::MenuItem( "Reload", "Ctrl+R" ) )
				{
					Reload();
				}
				if ( ImGui::MenuItem( "Save", "Ctrl+S" ) )
				{
					Save();
				}
				if ( ImGui::MenuItem( "Save as" ) )
				{
					SaveAs();
				}

				ImGui::Separator();

				if ( ImGui::MenuItem( "Reload shaders", "F11" ) )
				{
					onReloadShaders.Emmit();
				}

				if ( ImGui::MenuItem( "Reload icons", "F12" ) )
				{
					onReloadIcons.Emmit();
				}

				ImGui::Separator();

				if ( ImGui::MenuItem( "Exit" ) )
				{
					onExit.Emmit();
				}

				ImGui::EndMenu();
			}

			// View
			if ( ImGui::BeginMenu( "Tools" ) )
			{
				ImGui::Icon( ImGui::IMGUI16, { 16,16 } ); ImGui::SameLine();
				ImGui::MenuItem( "Imgui demo", nullptr, &m_showImguiDemoWindow );

				for ( size_t windowIndex = 0; windowIndex < m_editorWindows.size(); windowIndex++ )
				{
					EditorWindow* window = m_editorWindows[ windowIndex ];
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
				if ( ImGui::MenuItem( "show hull", nullptr, &m_showHull ) ) {}
				if ( ImGui::MenuItem( "show AABB", nullptr, &m_showAABB ) ) {}
				if ( ImGui::MenuItem( "show wireframe", nullptr, &m_showWireframe ) ) {}
				if ( ImGui::MenuItem( "show normals", nullptr, &m_showNormals ) ) {}
				if( ImGui::MenuItem(  "show lights", nullptr, &m_showLights ) ) {}
				
				ImGui::EndMenu();
			}

			// Grid
			if ( ImGui::BeginMenu( "Grid" ) )
			{
				ImGui::PushItemWidth( 150.f );
				ImGui::MenuItem( "visible", nullptr, &m_editorGrid->isVisible );
				ImGui::DragFloat( "spacing", &m_editorGrid->spacing, 0.25f, 0.f, 100.f );
				ImGui::DragInt( "lines count", &m_editorGrid->linesCount, 1.f, 0, 1000 );
				ImGui::ColorEdit3( "color", &m_editorGrid->color[ 0 ], ImGui::fanColorEditFlags );
				ImGui::DragFloat3( "offset", &m_editorGrid->offset[ 0 ] );
				ImGui::PopItemWidth();

				ImGui::EndMenu();
			}

			// Framerate
			ImGui::SameLine( ImGui::GetWindowWidth() - 60 );
			if ( ImGui::BeginMenu( std::to_string( Time::Get().GetRealFramerate() ).c_str(), false ) ) { ImGui::EndMenu(); }
			ImGui::FanToolTip( " Framerate. (Right click to set)" );

			if ( ImGui::IsItemClicked( 1 ) )
			{
				ImGui::OpenPopup( "main_menu_bar_set_fps" );
			}

			// Framerate set popup
			if ( ImGui::BeginPopup( "main_menu_bar_set_fps" ) )
			{
				Game& game = m_world->GetSingletonComponent<Game>();

				ImGui::PushItemWidth( 80.f );
				float maxFps = 1.f / Time::Get().GetRenderDelta();
				if ( ImGui::DragFloat( "fps", &maxFps, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					Time::Get().SetRenderDelta( maxFps < 1.f ? 1.f : 1.f / maxFps );
				}
				float maxLogicFrequency = 1.f / game.logicDelta;
				if ( ImGui::DragFloat( "logic frequency", &maxLogicFrequency, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					game.logicDelta = maxLogicFrequency < 1.f ? 1.f : 1.f / maxLogicFrequency;
				}
				float maxPhysicsFrequency = 1.f / Time::Get().GetPhysicsDelta();
				if ( ImGui::DragFloat( "physics frequency", &maxPhysicsFrequency, 1.f, 1.f, 3000.f, "%.f" ) )
				{
					Time::Get().SetPhysicsDelta( maxPhysicsFrequency < 1.f ? 1.f : 1.f / maxPhysicsFrequency );
				}
				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}

		} ImGui::EndMainMenuBar();


		// Open scene popup
		if ( m_openNewScenePopupLater == true )
		{
			m_openNewScenePopupLater = false;
			ImGui::OpenPopup( "New scene" );
		}

		// Open scene popup
		if ( m_openLoadScenePopupLater == true )
		{
			m_openLoadScenePopupLater = false;
			ImGui::OpenPopup( "Open scene" );
		}

		// Save scene popup
		if ( m_openSaveScenePopupLater == true )
		{
			m_openSaveScenePopupLater = false;
			ImGui::OpenPopup( "Save scene" );
		}

		DrawModals();

	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::DrawModals()
	{
		Scene& scene = m_world->GetSingletonComponent<Scene>();

		// New scene
		if ( ImGui::FanSaveFileModal( "New scene", RenderGlobal::s_sceneExtensions, m_pathBuffer ) )
		{		
			scene.New();
			scene.path = m_pathBuffer.string();
		}

		// Open scenes
		if ( ImGui::FanLoadFileModal( "Open scene", m_sceneExtensionFilter, m_pathBuffer ) )
		{
			Debug::Get() << Debug::Severity::log << "loading scene: " << m_pathBuffer.string() << Debug::Endl();
			scene.LoadFrom( m_pathBuffer.string() );
		}

		// Save scene
		if ( ImGui::FanSaveFileModal( "Save scene", RenderGlobal::s_sceneExtensions, m_pathBuffer ) )
		{
			scene.path = m_pathBuffer.string();
			Debug::Get() << Debug::Severity::log << "saving scene: " << scene.path << Debug::Endl();
			scene.Save();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::New()
	{
		Game& game = m_world->GetSingletonComponent<Game>();
		if( game.state != Game::STOPPED )
		{
			Debug::Warning() << "creating scenes is disabled in play mode" << Debug::Endl();
			return;
		}

		m_pathBuffer = "./content/scenes/";
		m_openNewScenePopupLater = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Open()
	{
		Game& game = m_world->GetSingletonComponent<Game>();
		if( game.state != Game::STOPPED )
		{
			Debug::Warning() << "loading scenes is disabled in play mode" << Debug::Endl();
			return;
		}

		m_pathBuffer = "./content/scenes/";
		m_openLoadScenePopupLater = true;
	}

	//================================================================================================================================
	// reload the scene
	//================================================================================================================================
	void MainMenuBar::Reload()
	{
		Game& game = m_world->GetSingletonComponent<Game>();
		Scene& scene = m_world->GetSingletonComponent<Scene>();

		if( scene.path.empty() )
		{
			Debug::Warning( "you cannot reload a scene that is not saved." );
			return;
		}

		if( game.state == Game::STOPPED )
		{
			// save old camera transform
			const EntityID oldCameraID = m_world->GetEntityID( scene.mainCamera->handle );
			btTransform oldCameraTransform = m_world->GetComponent<Transform>( oldCameraID ).transform;

			// save old selection
			SceneNode* prevSelectionNode = m_editorSelection.GetSelectedSceneNode();
			const uint32_t prevSelectionID = prevSelectionNode != nullptr ? prevSelectionNode->uniqueID : 0;

			Debug::Get() << Debug::Severity::log << "loading scene: " << scene.path << Debug::Endl();
			scene.LoadFrom( scene.path );

			// restore camera
			const EntityID newCameraID = m_world->GetEntityID( scene.mainCamera->handle );
			m_world->GetComponent<Transform>( newCameraID ).transform = oldCameraTransform;

			// restore selection
			if( prevSelectionID != 0 )
			{
				auto it = scene.nodes.find( prevSelectionID );
				if( it != scene.nodes.end() )
				{
					m_editorSelection.SetSelectedSceneNode( it->second );
				}
			}
		}
		else
		{
			Debug::Warning() << "reload is disabled in play mode" << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Save()
	{
		Game& game = m_world->GetSingletonComponent<Game>();
		Scene& scene = m_world->GetSingletonComponent<Scene>();

		if( game.state != Game::STOPPED )
		{
			Debug::Warning() << "saving is disabled in play mode" << Debug::Endl();
			return;
		}

		if ( ! scene.path.empty() )
		{
			Debug::Get() << Debug::Severity::log << "saving scene: " << scene.path << Debug::Endl();
			scene.Save();
		}
		else
		{
			SaveAs();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::SaveAs()
	{
		m_pathBuffer = "./content/scenes/";
		m_openSaveScenePopupLater = true;
	}
}