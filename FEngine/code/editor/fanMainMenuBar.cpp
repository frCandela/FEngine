#include "fanGlobalIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"
#include "editor/windows/fanSceneWindow.h"
#include "editor/windows/fanInspectorWindow.h"
#include "editor/windows/fanPreferencesWindow.h"
#include "editor/windows/fanConsoleWindow.h"	
#include "core/files/fanSerializedValues.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "editor/fanModals.h"
#include "scene/fanScene.h"

#include "renderer/fanRenderer.h"
#include "fanEngine.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar() :
		m_showImguiDemoWindow(true)
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
	//================================================================================================================================
	void MainMenuBar::Draw() {
		Engine &	engine = Engine::GetEngine();
		Renderer &	renderer = engine.GetRenderer();

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

				if (ImGui::MenuItem("Reload shaders", "F5")) {
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
			if (ImGui::BeginMenu("Grid"))
			{
				Engine::EditorGrid  gridData = engine.GetEditorGrid();
				if (ImGui::Checkbox("is visible", &gridData.isVisible)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::DragFloat("spacing", &gridData.spacing, 0.25f, 0.f, 100.f)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::DragInt("lines count", &gridData.linesCount, 1.f, 0, 1000)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::ColorEdit3("color", &gridData.color[0], gui::colorEditFlags)) {
					engine.SetEditorGrid(gridData);
				}
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

		if (Keyboard::IsKeyPressed(GLFW_KEY_DELETE)) {
			Entity * entity = Engine::GetEngine().GetSelectedentity();
			if (entity != nullptr) {
				Engine::GetEngine().GetScene().DeleteEntity(entity);
			}
		}

		if (Keyboard::IsKeyPressed(GLFW_KEY_F5)) {
			Engine::GetEngine().GetRenderer().ReloadShaders();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::DrawModals() {
		Engine &	engine = Engine::GetEngine();

		// New scene
		if (gui::SaveFileModal("New scene", GlobalValues::s_sceneExtensions, m_pathBuffer, m_extensionIndexBuffer)) {
			Scene & scene = engine.GetScene();
			scene.New();
			scene.SetPath(m_pathBuffer.string());
		}

		// Open scene
		if (gui::LoadFileModal("Open scene", m_sceneExtensionFilter, m_pathBuffer)) {
			Scene & scene = engine.GetScene();
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
		Scene & scene = Engine::GetEngine().GetScene();
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