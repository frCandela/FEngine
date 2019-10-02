#include "fanGlobalIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"
#include "editor/windows/fanSceneWindow.h"
#include "editor/windows/fanInspectorWindow.h"
#include "editor/windows/fanPreferencesWindow.h"
#include "editor/windows/fanConsoleWindow.h"	
#include "editor/fanEditorGrid.h"
#include "core/files/fanSerializedValues.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "editor/fanModals.h"
#include "scene/fanScene.h"
#include "core/fanTime.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar( Scene & _scene, EditorGrid & _editorGrid ) :
		m_scene( _scene )
		,m_editorGrid( _editorGrid )
		,m_showImguiDemoWindow(true)
		, m_showAABB(false)
		, m_showHull(false)
		, m_showWireframe(false)
		, m_showNormals(false)
		, m_sceneExtensionFilter(GlobalValues::s_sceneExtensions) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Initialize() {
		SerializedValues::Get().GetValue("show_imguidemo", m_showImguiDemoWindow);
	}

	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::~MainMenuBar() {
		SerializedValues::Get().SetValue("show_imguidemo", m_showImguiDemoWindow);
	}

	//================================================================================================================================
	// TODO Replace this with a vector
	//================================================================================================================================
	void MainMenuBar::SetWindows( EditorWindow * _renderWindow,
								  EditorWindow * _sceneWindow,
								  EditorWindow * _inspector,
								  EditorWindow * _preferences,
								  EditorWindow * _console )
	{
		m_renderWindow = _renderWindow;
		m_sceneWindow = _sceneWindow;
		m_inspector = _inspector;
		m_preferences= _preferences;
		m_console = _console;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Draw() {
		if (m_showImguiDemoWindow) {
			ImGui::ShowDemoWindow(&m_showImguiDemoWindow);
		}
		if (ImGui::BeginMainMenuBar())
		{
			// File
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {
					New();
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					Open();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					Save();
				}
				if (ImGui::MenuItem("Save as")) {
					SaveAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Reload shaders", "F5")) {
					onReloadShaders.Emmit();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) {
					onExit.Emmit();
				}

				ImGui::EndMenu();
			}

			// View
			if (ImGui::BeginMenu("View"))
			{
				bool showPostprocessWindow = m_renderWindow->IsVisible();
				if (ImGui::Checkbox("Rendering", &showPostprocessWindow)) {
					m_renderWindow->SetVisible(showPostprocessWindow);
				}
				bool showSceneWindow = m_sceneWindow->IsVisible();
				if (ImGui::Checkbox("Scene", &showSceneWindow)) {
					m_sceneWindow->SetVisible(showSceneWindow);
				}
				bool showInspector = m_inspector->IsVisible();
				if (ImGui::Checkbox("Inspector", &showInspector)) {
					m_inspector->SetVisible(showInspector);
				}
				bool showPreferences = m_preferences->IsVisible();
				if (ImGui::Checkbox("Preferences", &showPreferences)) {
					m_preferences->SetVisible(showPreferences);
				}
				bool showConsole = m_console->IsVisible();
				if (ImGui::Checkbox("Console", &showConsole)) {
					m_console->SetVisible(showConsole);
				}

				ImGui::Separator();
				ImGui::Checkbox("Imgui demo", &m_showImguiDemoWindow);

				ImGui::EndMenu();
			}

			// Editor
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::Checkbox("show hull", &m_showHull)) {}
				if (ImGui::Checkbox("show AABB", &m_showAABB)) {}
				if (ImGui::Checkbox("show Wireframe", &m_showWireframe)) {}
				if (ImGui::Checkbox("show Normals", &m_showNormals)) {}
				ImGui::EndMenu();
			}

			// Editor
			if (ImGui::BeginMenu("Grid"))			{
				ImGui::Checkbox("is visible", &m_editorGrid.isVisible);
				ImGui::DragFloat("spacing", &m_editorGrid.spacing, 0.25f, 0.f, 100.f);
				ImGui::DragInt("lines count", &m_editorGrid.linesCount, 1.f, 0, 1000);
				ImGui::ColorEdit3("color", &m_editorGrid.color[0], gui::colorEditFlags);
				ImGui::EndMenu();
			}

			// Calculates average fps during the last second
			float frameTime = Time::Get().GetFrameTime();
			m_fpsCounter.sum += frameTime;
			++ m_fpsCounter.count;
			if ( m_fpsCounter.sum >= 1 ) { // every 1s
				m_fpsCounter.fps = static_cast<float>( m_fpsCounter.count) / m_fpsCounter.sum;
				m_fpsCounter.count = 0;
				m_fpsCounter.sum = 0.f;
			}

			// Framerate
			std::stringstream ss;
			ss << std::fixed  << std::setprecision(0) << m_fpsCounter.fps;
			ImGui::SameLine(ImGui::GetWindowWidth() - 60);
			if ( ImGui::BeginMenu( ss.str().c_str(), false ) ) {ImGui::EndMenu();}
			if ( ImGui::IsItemClicked( 1 ) ) {
				ImGui::OpenPopup( "main_menu_bar_set_fps" );
			}

			// Framerate set popup
			if ( ImGui::BeginPopup( "main_menu_bar_set_fps" ) ) {
				ImGui::PushItemWidth( 80.f );
				float maxFps = 1.f / Time::Get().GetDelta();
				if ( ImGui::DragFloat( "fps", &maxFps, 1.f, 1.f, 3000.f, "%.f" ) ) {
					Time::Get().SetDelta( maxFps < 1.f ? 1.f : 1.f / maxFps );
				}
				ImGui::EndPopup();
			}



		} ImGui::EndMainMenuBar();

		ProcessKeyboardShortcuts();

		// Open scene popup
		if (m_openNewScenePopupLater == true) {
			m_openNewScenePopupLater = false;
			ImGui::OpenPopup("New scene");
		}

		// Open scene popup
		if (m_openLoadScenePopupLater == true) {
			m_openLoadScenePopupLater = false;
			ImGui::OpenPopup("Open scene");
		}

		// Save scene popup
		if (m_openSaveScenePopupLater == true) {
			m_openSaveScenePopupLater = false;
			ImGui::OpenPopup("Save scene");
		}

		DrawModals();
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::ProcessKeyboardShortcuts() {

		if (Keyboard::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && Keyboard::IsKeyPressed(GLFW_KEY_O)) {
			Open();
		}

		if (Keyboard::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && Keyboard::IsKeyPressed(GLFW_KEY_S)) {
			Save();
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::DrawModals() {
		// New scene
		if (gui::SaveFileModal("New scene", GlobalValues::s_sceneExtensions, m_pathBuffer, m_extensionIndexBuffer)) {
			m_scene.New();
			m_scene.SetPath(m_pathBuffer.string());
		}

		// Open scene
		if (gui::LoadFileModal("Open scene", m_sceneExtensionFilter, m_pathBuffer)) {
			m_scene.LoadFrom(m_pathBuffer.string());
		}

		// Save scene
		if (gui::SaveFileModal("Save scene", GlobalValues::s_sceneExtensions, m_pathBuffer, m_extensionIndexBuffer)) {
			m_scene.SetPath(m_pathBuffer.string());
			m_scene.Save();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::New() {
		m_extensionIndexBuffer = 0;
		m_pathBuffer = "./content/scenes/";
		m_openNewScenePopupLater = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Open() {
		m_pathBuffer = "./content/scenes/";
		m_openLoadScenePopupLater = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Save() {		
		if ( m_scene.HasPath() ) {
			m_scene.Save();
		}
		else {
			SaveAs();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::SaveAs() {
		m_pathBuffer = "./content/scenes/";
		m_extensionIndexBuffer = 0;
		m_openSaveScenePopupLater = true;
	}
}