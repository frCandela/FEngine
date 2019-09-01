#include "fanGlobalIncludes.h"

#include "editor/windows/fanSceneWindow.h"
#include "renderer/fanRenderer.h"
#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanTransform.h"
#include "core/fanInput.h"

#include "fanEngine.h"

namespace fan
{
	namespace editor {

		//================================================================================================================================
		//================================================================================================================================
		SceneWindow::SceneWindow() :
			m_textBuffer({ "new_entity" }) {

		}

		//================================================================================================================================
		//================================================================================================================================
		void SceneWindow::Draw() {
			if (IsVisible() == true) {
				fan::Engine & engine = fan::Engine::GetEngine();
				scene::Scene & scene = engine.GetScene();

				bool isVisible = IsVisible();
				if (ImGui::Begin("Scene", &isVisible)) {

					bool newentity = false;
					if (ImGui::BeginPopupContextWindow("PopupContextWindowNewentity"))
					{
						if (ImGui::Selectable("New entity")) {
							newentity = true;
						}
						ImGui::EndPopup();
					}
					if (newentity) {
						ImGui::OpenPopup("New entity");
					} NewentityModal();

					ImGui::Text(scene.GetName().c_str());
					ImGui::Separator();

					// Show entities tree
					bool popupOneTime = true;
					const std::vector< scene::Entity * > & entities = scene.GetEntities();
					for (int entityIndex = 0; entityIndex < entities.size(); entityIndex++)
					{
						scene::Entity * entity = entities[entityIndex];

						ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | (entity == engine.GetSelectedentity() ? ImGuiTreeNodeFlags_Selected : 0);
						bool nodeOpen = ImGui::TreeNodeEx(entity->GetName().c_str(), node_flags);
						if (ImGui::IsItemClicked()) {
							engine.SetSelectedentity(entity);
						}

						if (popupOneTime && ImGui::BeginPopupContextItem(scene.GetName().c_str()))
						{
							if (entity->HasFlag(scene::Entity::NO_DELETE) == false && ImGui::Selectable("Delete")) {
								scene.DeleteEntity(entity);
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

		void SceneWindow::NewentityModal()
		{
			fan::Engine & engine = fan::Engine::GetEngine();
			scene::Scene & scene = engine.GetScene();

			ImGui::SetNextWindowSize(ImVec2(200, 200));
			if (ImGui::BeginPopupModal("New entity"))
			{
				ImGui::InputText("Name ", m_textBuffer.data(), m_textBuffer.size());
				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();
				ImGui::SameLine();
				if (ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER))
				{
					//Create new entity 
					scene::Entity* newentity = scene.CreateEntity(m_textBuffer.data());
					newentity->AddComponent<scene::Transform>();
					engine.SetSelectedentity(newentity);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
}