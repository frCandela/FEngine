#include "fanGlobalIncludes.h"
#include "editor/windows/fanInspectorWindow.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/components/fanMaterial.h"
#include "editor/components/fanFPSCamera.h"
#include "editor/fanDragnDrop.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "core/time/fanProfiler.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() :
		EditorWindow("inspector", ImGui::IconType::INSPECTOR )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui() {
		SCOPED_PROFILE( inspector )
		if ( m_gameobjectSelected != nullptr)
		{
			// gameobject gui
			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			m_gameobjectSelected->OnGui();
			int componentCount = 0;			
			const std::vector<Component*> & components = m_gameobjectSelected->GetComponents();
			for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
				Component * component = components[componentIndex];

				ImGui::Separator();

				// Icon
				ImGui::Icon( component->GetIcon(), { 19,19 } ); ImGui::SameLine();
				ImGui::FanBeginDragDropSourceComponent( component,  ImGuiDragDropFlags_SourceAllowNullID);

				// Actor "enable" checkbox
				if (component->IsActor()) {	// TODO : use type info when type info deals with inheritance
					ImGui::PushID((int*)component);
					Actor * actor = static_cast<Actor*>(component);
					bool enabled = actor->IsEnabled();
					if (ImGui::Checkbox("", &enabled)) {
						actor->SetEnabled(enabled);
					}
					ImGui::SameLine();
					ImGui::PopID();
				}

				// Delete button	
				ImGui::Text( component->GetName());
				ImGui::FanBeginDragDropSourceComponent( component,  ImGuiDragDropFlags_SourceAllowNullID);

				if (component->IsRemovable()) {
					std::stringstream ss;
					ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
					ImGui::SameLine(ImGui::GetWindowWidth() - 40);
					if (ImGui::Button(ss.str().c_str())) {
						m_gameobjectSelected->RemoveComponent(component);
						component = nullptr;
					}
				}
				
				// Draw component
				if (component != nullptr) {
					component->OnGui();
				}
			}			
			ImGui::Separator();
			//Add component button
			if (ImGui::Button("Add component"))
				ImGui::OpenPopup("New component");

			NewComponentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup() {

		if (ImGui::BeginPopup("New component"))
		{
			std::vector< const void *> components = TypeInfo::Get().GetInstancesWithFlags( TypeInfo::EDITOR_COMPONENT );
			for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
				const Component * component = static_cast< const Component *> ( components[componentIndex] );

				ImGui::Icon( component->GetIcon(), { 19,19 } ); ImGui::SameLine();
				if (ImGui::MenuItem(component->GetName())) {
					// Create new Component 
					m_gameobjectSelected->AddComponent(component->GetType());
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
}