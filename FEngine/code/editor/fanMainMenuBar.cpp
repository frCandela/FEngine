#include "fanIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"
#include "editor/windows/fanSceneWindow.h"
#include "editor/windows/fanInspectorWindow.h"
#include "editor/windows/fanPreferencesWindow.h"
#include "util/fanSerializedValues.h"

#include "vulkan/vkRenderer.h"
#include "fanEngine.h"

namespace editor {	

	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar() :
		m_showImguiDemoWindow( true )
		, m_showAABB(false)
		, m_showWireframe(false) {
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
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Draw() {
		fan::Engine &	engine =	fan::Engine::GetEngine();
		vk::Renderer &	renderer = vk::Renderer::GetRenderer();

		if (m_showImguiDemoWindow) {
			ImGui::ShowDemoWindow(&m_showImguiDemoWindow);
		}
		
		if (ImGui::BeginMainMenuBar())
		{
			// FILE
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Reload shaders")) {
					renderer.ReloadShaders();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")){
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

				ImGui::Separator();
				ImGui::Checkbox("Imgui demo", &m_showImguiDemoWindow);

				ImGui::EndMenu();
			}

			// EDITOR
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::Checkbox("show AABB", &m_showAABB)) {}
				if (ImGui::Checkbox("show Wireframe", &m_showWireframe)) {}
				ImGui::EndMenu();
			}

			/////////////////////
			ImGui::EndMainMenuBar();
		} 



	}
}