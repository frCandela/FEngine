#include "fanGlobalIncludes.h"

#include "editor/windows/fanInspectorWindow.h"
#include "fanGlobalValues.h"
#include "scene/fanEntity.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanMaterial.h"
#include "editor/components/fanFPSCamera.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "fanEngine.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		InspectorWindow::InspectorWindow() : 
			Window("inspector") 
		{		
		}

		//================================================================================================================================
		//================================================================================================================================
		void InspectorWindow::Draw() {
			if (IsVisible() == true) {
				fan::Engine & engine = fan::Engine::GetEngine();
				scene::Entity * const selection = engine.GetSelectedentity();

				bool visible = IsVisible();
				ImGui::Begin("Inspector", &visible);
				if (selection != nullptr)
				{
					// entity gui
					selection->OnGui();
					int componentCount = 0;

					const std::vector<scene::Component*> & components = selection->GetComponents();
					for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
						scene::Component * component = components[componentIndex];

						ImGui::Separator();

						if (component->IsActor()) {	// TODO : use type info when type info deals with inheritance
							ImGui::PushID((int*)component);
							scene::Actor * actor = static_cast<scene::Actor*>(component);
							bool enabled = actor->IsEnabled();
							if (ImGui::Checkbox("", &enabled)) {
								actor->SetEnabled(enabled);
							}
							ImGui::SameLine();
							ImGui::PopID();
						}

						// Delete button
						ImGui::Text(component->GetName());
						if (component->IsRemovable()) {
							std::stringstream ss;
							ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
							ImGui::SameLine(ImGui::GetWindowWidth() - 40);
							if (ImGui::Button(ss.str().c_str())) {
								selection->DeleteComponent(component);
								component = nullptr;
							}
						}
						if (component != nullptr) {
							component->OnGui();
						}
					}
					ImGui::Separator();
					ImGui::SameLine();
					//Add component button
					if (ImGui::Button("Add component"))
						ImGui::OpenPopup("New component");

					NewComponentPopup();

				} ImGui::End();
				SetVisible(visible);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void InspectorWindow::NewComponentPopup() {

			if (ImGui::BeginPopup("New component"))
			{
				fan::Engine & engine = fan::Engine::GetEngine();
				scene::Entity * const selection = engine.GetSelectedentity();
				
				std::vector<scene::Component *>& components = ComponentsRegister::GetComponents();
				for (int componentIndex = 0; componentIndex < components.size() ; componentIndex++)		{					
					scene::Component * component = components[componentIndex];
					if (ImGui::MenuItem(component->GetName())) {
						// Create new Component 
						selection->AddComponent(component->GetType());
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		}
	}
}