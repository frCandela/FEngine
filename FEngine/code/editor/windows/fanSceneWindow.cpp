#include "fanIncludes.h"

#include "editor/windows/fanSceneWindow.h"
#include "vulkan/vkRenderer.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "util/fanInput.h"

#include "fanEngine.h"

namespace editor {

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow() :
		m_textBuffer({ "new gameobject" }) {

	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::Draw() {
		if (IsVisible() == true) {
			fan::Engine & engine = fan::Engine::GetEngine();
			scene::Scene & scene = engine.GetScene();

			bool isVisible = IsVisible();
			if (ImGui::Begin("Scene", &isVisible)) {

				bool newGameobject = false;
				if (ImGui::BeginPopupContextWindow("PopupContextWindowNewGameobject"))
				{
					if (ImGui::Selectable("New Gameobject")) {
						newGameobject = true;
					}
					ImGui::EndPopup();
				}
				if (newGameobject) {
					ImGui::OpenPopup("New Gameobject");
				} NewGameobjectModal();

				ImGui::Text(scene.GetName().c_str());
				ImGui::Separator();

				// Show gameobjects tree
				bool popupOneTime = true;
				const std::vector< scene::Gameobject * > & gameobjects = scene.GetGameObjects();
				for (int gameobjectIndex = 0; gameobjectIndex < gameobjects.size(); gameobjectIndex++)
				{
					scene::Gameobject * gameobject = gameobjects[gameobjectIndex];

					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | (gameobject == engine.GetSelectedGameobject() ? ImGuiTreeNodeFlags_Selected : 0);
					bool nodeOpen = ImGui::TreeNodeEx(gameobject->GetName().c_str(), node_flags);
					if (ImGui::IsItemClicked()) {
						engine.SetSelectedGameobject(gameobject);
					}

					if (popupOneTime && ImGui::BeginPopupContextItem(scene.GetName().c_str()))
					{
						if ( gameobject->IsRemovable() && ImGui::Selectable("Delete")) {	
							scene.DeleteGameobject(gameobject);
						}
						ImGui::EndPopup();
						popupOneTime = false;
					}

					if (nodeOpen) {
						ImGui::TreePop();
					}
				}

			} ImGui::End();
			SetVisible(isVisible);
		}
	}

	void SceneWindow::NewGameobjectModal()
	{
		fan::Engine & engine = fan::Engine::GetEngine();
		scene::Scene & scene = engine.GetScene();

		ImGui::SetNextWindowSize(ImVec2(200, 200));
		if (ImGui::BeginPopupModal("New Gameobject"))
		{
			ImGui::InputText("Name ", m_textBuffer.data(), m_textBuffer.size());
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::SameLine();
			if (ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER))
			{
				//Create new gameobject 
				scene::Gameobject* newGameobject = scene.CreateGameobject(m_textBuffer.data());
				newGameobject->AddComponent<scene::Transform>();
				engine.SetSelectedGameobject(newGameobject);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}