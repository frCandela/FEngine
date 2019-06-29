#include "fanIncludes.h"

#include "editor/fanMainMenuBar.h"
#include "editor/fanRenderWindow.h"
#include "vulkan/vkRenderer.h"
#include "fanEngine.h"

namespace editor {
	MainMenuBar::MainMenuBar() :
		  m_showImguiDemoWindow		( true ){

	}

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
				ImGui::Checkbox("Imgui demo", &m_showImguiDemoWindow);

				bool showPostprocessWindow = engine.GetRenderWindow().IsVisible();
				if (ImGui::Checkbox("Rendering", &showPostprocessWindow)) {
					engine.GetRenderWindow().SetVisible(showPostprocessWindow);
				}
				
				ImGui::EndMenu();
			}

			/////////////////////
			ImGui::EndMainMenuBar();
		} 



	}
}