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

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow() :
		EditorWindow("scene")
	{
		m_textBuffer[0] = '\0';
		Scene::s_onSceneLoad.Connect(&SceneWindow::OnSceneLoad, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::~SceneWindow() {
		Scene::s_onSceneLoad.Disconnect(&SceneWindow::OnSceneLoad, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::OnGui() {
		Engine & engine = Engine::GetEngine();
		Scene & scene = engine.GetScene();

		ImGui::Text(scene.GetName().c_str());
		ImGui::Separator();

		Entity * entityRightClicked = nullptr;
		R_DrawSceneTree(scene.GetRoot(), entityRightClicked);
		m_expandSceneHierarchy = false;

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
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::R_DrawSceneTree(Entity * _entityDrawn, Entity* & _entityRightClicked) {
		Engine & engine = Engine::GetEngine();

		std::stringstream ss;
		ss << "##" << _entityDrawn; // Unique id

		if (ImGui::IsWindowAppearing() || m_expandSceneHierarchy == true) {
			ImGui::SetNextTreeNodeOpen(true);
		}
		bool isOpen = ImGui::TreeNode(ss.str().c_str());

		// Entity dragndrop target empty selectable -> place dragged below
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("node_test")) {
				assert(payload->DataSize == sizeof(Entity**));
				Entity * payloadNode = *(Entity**)payload->Data;
				if( payloadNode != _entityDrawn ) {
					payloadNode->InsertBelow(_entityDrawn);
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		bool selected = (_entityDrawn == engine.GetSelectedentity());

		// Draw entity empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _entityDrawn->GetName() << "##" << _entityDrawn; // Unique id
		if (ImGui::Selectable(ss2.str().c_str(), &selected)) {
			engine.SetSelectedEntity(_entityDrawn);
		}
		if (ImGui::IsItemClicked(1)) {
			_entityRightClicked = _entityDrawn;
		}

		// Entity dragndrop source = selectable -^
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("node_test", &_entityDrawn, sizeof(Entity**));
			ImGui::Text((_entityDrawn->GetName()).c_str());
			ImGui::EndDragDropSource();
		}

		// Entity dragndrop target entity name -> place as child
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("node_test")) {
				assert(payload->DataSize == sizeof(Entity**));
				Entity * payloadNode = *(Entity**)payload->Data;
				payloadNode->SetParent(_entityDrawn);
			}
			ImGui::EndDragDropTarget();
		}
		if (isOpen) {
			const std::vector<Entity*>& childs = _entityDrawn->GetChilds();
			for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
				Entity * child = childs[childIndex];
				R_DrawSceneTree(child, _entityRightClicked);
			}

			ImGui::TreePop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::NewEntityModal()
	{
		Engine & engine = Engine::GetEngine();
		Scene & scene = engine.GetScene();

		ImGui::SetNextWindowSize(ImVec2(200, 200));
		if (ImGui::BeginPopupModal("New entity"))
		{
			if (ImGui::IsWindowAppearing()) {
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if (ImGui::InputText("Name ", m_textBuffer, IM_ARRAYSIZE(m_textBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterPressed = true;
			}
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE, false)) {
				m_lastEntityRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Ok") || enterPressed)
			{
				if (std::string(m_textBuffer) != "") {
					//Create new entity 
					Entity* newentity = scene.CreateEntity(m_textBuffer, m_lastEntityRightClicked);
					newentity->AddComponent<Transform>();
					engine.SetSelectedEntity(newentity);
					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
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
			if (ImGui::IsWindowAppearing()) {
				strcpy_s(m_textBuffer, 32, m_lastEntityRightClicked->GetName().c_str());
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if (ImGui::InputText("New Name ", m_textBuffer, IM_ARRAYSIZE(m_textBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterPressed = true;
			}
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE, false)) {
				m_lastEntityRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Ok") || ImGui::IsKeyPressed(GLFW_KEY_ENTER, false) || enterPressed)
			{
				if (std::string(m_textBuffer) != "") {
					m_lastEntityRightClicked->SetName(m_textBuffer);
					m_lastEntityRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
}