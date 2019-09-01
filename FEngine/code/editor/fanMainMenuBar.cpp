#include "fanGlobalIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "fanGlobalValues.h"
#include "editor/windows/fanRenderWindow.h"
#include "editor/windows/fanSceneWindow.h"
#include "editor/windows/fanInspectorWindow.h"
#include "editor/windows/fanPreferencesWindow.h"
#include "editor/windows/fanConsoleWindow.h"	
#include "core/files/fanSerializedValues.h"
#include "core/fanInput.h"
#include "editor/fanModals.h"
#include "scene/fanScene.h"

#include "renderer/fanRenderer.h"
#include "fanEngine.h"

namespace fan
{
	namespace editor {

		//================================================================================================================================
		//================================================================================================================================
		MainMenuBar::MainMenuBar() :
			m_showImguiDemoWindow(true)
			, m_showAABB(false)
			, m_showWireframe(false)
			, m_showNormals(false)
			, m_sceneExtensionFilter(GlobalValues::s_sceneExtensions) {
		}

		//================================================================================================================================
		//================================================================================================================================
		void MainMenuBar::Initialize() {
			fan::Engine & engine = fan::Engine::GetEngine();
			fan::SerializedValues & editorValues = engine.GetEditorValues();
			bool tmpValue;

			editorValues.Get("mainMenuBar_show_imguidemo", m_showImguiDemoWindow);

			if (editorValues.Get("mainMenuBar_show_postprocess", tmpValue) == true) {
				engine.GetRenderWindow().SetVisible(tmpValue);
			}

			if (editorValues.Get("mainMenuBar_show_scene", tmpValue) == true) {
				engine.GetSceneWindow().SetVisible(tmpValue);
			}

			if (editorValues.Get("mainMenuBar_show_inspector", tmpValue) == true) {
				engine.GetInspectorWindow().SetVisible(tmpValue);
			}

			if (editorValues.Get("mainMenuBar_show_preferences", tmpValue) == true) {
				engine.GetPreferencesWindow().SetVisible(tmpValue);
			}

			if (editorValues.Get("mainMenuBar_show_console", tmpValue) == true) {
				engine.GetConsoleWindow().SetVisible(tmpValue);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		MainMenuBar::~MainMenuBar() {
			fan::Engine & engine = fan::Engine::GetEngine();
			fan::SerializedValues & editorValues = engine.GetEditorValues();

			editorValues.Set("mainMenuBar_show_imguidemo", m_showImguiDemoWindow);
			editorValues.Set("mainMenuBar_show_postprocess", engine.GetRenderWindow().IsVisible());
			editorValues.Set("mainMenuBar_show_scene", engine.GetSceneWindow().IsVisible());
			editorValues.Set("mainMenuBar_show_inspector", engine.GetInspectorWindow().IsVisible());
			editorValues.Set("mainMenuBar_show_preferences", engine.GetPreferencesWindow().IsVisible());
			editorValues.Set("mainMenuBar_show_console", engine.GetConsoleWindow().IsVisible());

		}

		//================================================================================================================================
		//================================================================================================================================
		void MainMenuBar::Draw() {
			fan::Engine &	engine = fan::Engine::GetEngine();
			Renderer &	renderer = Renderer::GetRenderer();

			if (m_showImguiDemoWindow) {
				ImGui::ShowDemoWindow(&m_showImguiDemoWindow);
			}

			if (ImGui::BeginMainMenuBar())
			{
				// FILE
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

					if (ImGui::MenuItem("Reload shaders")) {
						renderer.ReloadShaders();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Exit")) {
						engine.Exit();
					}

					ImGui::EndMenu();
				}

				// View
				if (ImGui::BeginMenu("View"))
				{
					bool showPostprocessWindow = engine.GetRenderWindow().IsVisible();
					if (ImGui::Checkbox("Rendering", &showPostprocessWindow)) {
						engine.GetRenderWindow().SetVisible(showPostprocessWindow);
					}
					bool showSceneWindow = engine.GetSceneWindow().IsVisible();
					if (ImGui::Checkbox("Scene", &showSceneWindow)) {
						engine.GetSceneWindow().SetVisible(showSceneWindow);
					}
					bool showInspector = engine.GetInspectorWindow().IsVisible();
					if (ImGui::Checkbox("Inspector", &showInspector)) {
						engine.GetInspectorWindow().SetVisible(showInspector);
					}
					bool showPreferences = engine.GetPreferencesWindow().IsVisible();
					if (ImGui::Checkbox("Preferences", &showPreferences)) {
						engine.GetPreferencesWindow().SetVisible(showPreferences);
					}
					bool showConsole = engine.GetConsoleWindow().IsVisible();
					if (ImGui::Checkbox("Console", &showConsole)) {
						engine.GetConsoleWindow().SetVisible(showConsole);
					}

					ImGui::Separator();
					ImGui::Checkbox("Imgui demo", &m_showImguiDemoWindow);

					ImGui::EndMenu();
				}

				// EDITOR
				if (ImGui::BeginMenu("Editor"))
				{
					if (ImGui::Checkbox("show AABB", &m_showAABB)) {}
					if (ImGui::Checkbox("show Wireframe", &m_showWireframe)) {}
					if (ImGui::Checkbox("show Normals", &m_showNormals)) {}
					ImGui::EndMenu();
				}

				/////////////////////
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
			fan::Engine &	engine = fan::Engine::GetEngine();

			// New scene
			if (gui::SaveFileModal("New scene", GlobalValues::s_sceneExtensions, m_pathBuffer, m_extensionIndexBuffer)) {
				scene::Scene & scene = engine.GetScene();
				scene.New();
				scene.SetPath(m_pathBuffer.string());
			}

			// Open scene
			if (gui::LoadFileModal("Open scene", m_sceneExtensionFilter, m_pathBuffer)) {
				scene::Scene & scene = engine.GetScene();
				scene.LoadFrom(m_pathBuffer.string());
			}

			// Save scene
			if (gui::SaveFileModal("Save scene", GlobalValues::s_sceneExtensions, m_pathBuffer, m_extensionIndexBuffer)) {
				engine.GetScene().SetPath(m_pathBuffer.string());
				engine.GetScene().Save();
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
			scene::Scene & scene = fan::Engine::GetEngine().GetScene();
			if (scene.HasPath()) {
				scene.Save();
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
}