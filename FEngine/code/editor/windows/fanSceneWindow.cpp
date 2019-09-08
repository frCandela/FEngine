#include "fanGlobalIncludes.h"

#include "editor/windows/fanSceneWindow.h"
#include "renderer/fanRenderer.h"
#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanTransform.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"

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
			bool isVisible = IsVisible();
			if (isVisible == true) {
				fan::Engine & engine = fan::Engine::GetEngine();
				scene::Scene & scene = engine.GetScene();
				
				if (ImGui::Begin("Scene", &isVisible)) {
					ImGui::Text(scene.GetName().c_str());
					ImGui::Separator();

					scene::Entity * entityRightClicked = nullptr;
					R_DrawSceneTree(scene.GetRoot(), entityRightClicked);

					if (entityRightClicked != nullptr) {
						ImGui::OpenPopup("scene_window_entity_rclicked");
						m_lastEntityRightClicked = entityRightClicked;
					}

					bool newEntityPopup = false;
					bool renameEntityPopup = false;
					if (ImGui::BeginPopup("scene_window_entity_rclicked")) {
						if (ImGui::Selectable("New entity")) {
							newEntityPopup = true;					
						} 
						if (ImGui::Selectable("Rename")) {
							renameEntityPopup = true;
						}
						ImGui::Separator();
						if (ImGui::Selectable("Delete")) {
							scene.DeleteEntity(m_lastEntityRightClicked);
						}
						ImGui::EndPopup();
					} 

					if (newEntityPopup) {
						ImGui::OpenPopup("New entity");
					} NewEntityModal();

					if (renameEntityPopup) {
						ImGui::OpenPopup("Rename entity");
					} RenameEntityModal();

				} ImGui::End();
				SetVisible(isVisible);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void SceneWindow::R_DrawSceneTree(scene::Entity * _entityDrawn, scene::Entity* & _entityRightClicked ) {
			fan::Engine & engine = fan::Engine::GetEngine();

			std::stringstream ss;
			ss << "##" << _entityDrawn; // Unique id

			bool isOpen = ImGui::TreeNode(ss.str().c_str());

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("node_test")) {
					assert(payload->DataSize == sizeof(scene::Entity**));
					scene::Entity * payloadNode = *(scene::Entity**)payload->Data;
					payloadNode->InsertBelow(_entityDrawn);
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			bool selected = (_entityDrawn == engine.GetSelectedentity());
			
			std::stringstream ss2;
			ss2 << _entityDrawn->GetName() <<"##" << _entityDrawn; // Unique id
			if (ImGui::Selectable(ss2.str().c_str(), &selected)) {
				engine.SetSelectedEntity(_entityDrawn);
			}
			if (ImGui::IsItemClicked(1)) {
				_entityRightClicked = _entityDrawn;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("node_test", &_entityDrawn, sizeof(scene::Entity**));
				ImGui::Text((_entityDrawn->GetName()).c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("node_test")) {
					assert(payload->DataSize == sizeof(scene::Entity**));
					scene::Entity * payloadNode = *(scene::Entity**)payload->Data;
					payloadNode->SetParent(_entityDrawn);
				}
				ImGui::EndDragDropTarget();
			}
			if (isOpen) {
				const std::vector<scene::Entity*>& childs = _entityDrawn->GetChilds();
				for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
					scene::Entity * child = childs[childIndex];
					R_DrawSceneTree(child, _entityRightClicked);
				}

				ImGui::TreePop();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void SceneWindow::NewEntityModal()
		{
			fan::Engine & engine = fan::Engine::GetEngine();
			scene::Scene & scene = engine.GetScene();

			ImGui::SetNextWindowSize(ImVec2(200, 200));
			if (ImGui::BeginPopupModal("New entity"))
			{
				ImGui::InputText("Name ", m_textBuffer.data(), m_textBuffer.size());
				if (ImGui::Button("Cancel")) {
					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER, true) || Keyboard::IsKeyPressed(GLFW_KEY_KP_ENTER, true))
				{
					//Create new entity 
					scene::Entity* newentity = scene.CreateEntity(m_textBuffer.data(), m_lastEntityRightClicked );
					newentity->AddComponent<scene::Transform>();
					engine.SetSelectedEntity(newentity);
					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}		
		
		//================================================================================================================================
		//================================================================================================================================
		void SceneWindow::RenameEntityModal()
		{
			ImGui::SetNextWindowSize(ImVec2(200, 200));
			if (ImGui::BeginPopupModal("Rename entity"))
			{
				ImGui::InputText("New Name ", m_textBuffer.data(), m_textBuffer.size());
				if (ImGui::Button("Cancel")) {
					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER, true) || Keyboard::IsKeyPressed(GLFW_KEY_KP_ENTER, true))
				{
					m_lastEntityRightClicked->SetName(m_textBuffer.data());
 					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
}