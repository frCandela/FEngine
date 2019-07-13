#include "fanIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"
#include "editor/windows/fanSceneWindow.h"
#include "editor/windows/fanInspectorWindow.h"
#include "editor/windows/fanPreferencesWindow.h"

#include "vulkan/vkRenderer.h"
#include "fanEngine.h"

namespace editor {	

	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar() :
		  m_showImguiDemoWindow		( true ){

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

			/////////////////////
			ImGui::EndMainMenuBar();
		} 



	}
}