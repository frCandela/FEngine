#include "fanGlobalIncludes.h"

#include "editor/windows/fanSceneWindow.h"
#include "renderer/fanRenderer.h"
#include "scene/fanScene.h"
#include "scene/fanEntity.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanDirectionalLight.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow( Scene * _scene ) :
		EditorWindow("Scene")
		, m_scene(_scene)
	{
		m_textBuffer[0] = '\0';
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::~SceneWindow() {}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::OnGui() {

		ImGui::Text(m_scene->GetName().c_str());
		ImGui::Separator();

		Entity * entityRightClicked = nullptr;
		R_DrawSceneTree( m_scene->GetRoot(), entityRightClicked);
		m_expandSceneHierarchy = false;

		if (entityRightClicked != nullptr) {
			ImGui::OpenPopup("scene_window_entity_rclicked");
			m_lastEntityRightClicked = entityRightClicked;
		}

		// Popup set entity when right clic
		bool newEntityPopup = false;
		bool renameEntityPopup = false;
		if (ImGui::BeginPopup("scene_window_entity_rclicked")) {

			// New entity 
			bool itemClicked = false;
			if (ImGui::BeginMenu("New entity")) {
				// Popup empty entity
				if ( ImGui::IsItemClicked() ) {
					itemClicked = true;
				}
				// Entities templates
				if ( ImGui::MenuItem( "Model" ) ) {
					Entity *  newIntity = m_scene->CreateEntity("new model", m_lastEntityRightClicked );
					newIntity->AddComponent<Transform>();
					newIntity->AddComponent<Model>();
					newIntity->AddComponent<Material>();
				}
				if ( ImGui::MenuItem( "Point light" ) ) {
					Entity *  newIntity = m_scene->CreateEntity( "new_point_light", m_lastEntityRightClicked );
					newIntity->AddComponent<Transform>();
					newIntity->AddComponent<PointLight>();
				}
				if ( ImGui::MenuItem( "Dir light" ) ) {
					Entity *  newIntity = m_scene->CreateEntity( "new_dir_light", m_lastEntityRightClicked );
					newIntity->AddComponent<Transform>();
					newIntity->AddComponent<DirectionalLight>();
				}
				ImGui::EndMenu();
			}
			if ( ImGui::IsItemClicked() ) {
				newEntityPopup = true;
			}

			// rename
			if (ImGui::Selectable("Rename")) {
				renameEntityPopup = true;
			}

			// delete
			ImGui::Separator();
			if (ImGui::Selectable("Delete")) {
				m_scene->DeleteEntity(m_lastEntityRightClicked);
			}
			ImGui::EndPopup();
		}

		// Modals
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
		bool selected = ( _entityDrawn == m_entitySelected );

		// Draw entity empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _entityDrawn->GetName() << "##" << _entityDrawn; // Unique id
		if (ImGui::Selectable(ss2.str().c_str(), &selected)) {
			onSelectEntity.Emmit( _entityDrawn );
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
					Entity* newentity = m_scene->CreateEntity(m_textBuffer, m_lastEntityRightClicked);
					newentity->AddComponent<Transform>();
					onSelectEntity.Emmit( newentity );
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