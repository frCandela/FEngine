#include "fanIncludes.h"

#include "editor/fanInspectorWindow.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "fanEngine.h"
#include "util/fanUtil.h"

namespace editor {
	InspectorWindow::InspectorWindow() {}

	void InspectorWindow::Draw() {
		fan::Engine & engine = fan::Engine::GetEngine();
		//scene::Scene & scene = engine.GetScene();
		scene::Gameobject * const selection = engine.GetSelectedGameobject();

		bool visible = IsVisible();
		ImGui::Begin("Inspector", &visible );
		if ( selection != nullptr )
		{
			// Gameobject gui
			ImGui::Text("GameObject : %s", selection->GetName().c_str());
			int componentCount = 0;

			const std::vector<scene::Component*> & components = selection->GetComponents();
			for (int componentIndex = 0; componentIndex < components.size() ; componentIndex++) {
				scene::Component * const component =  components[componentIndex];

				ImGui::Separator();

				// Delete button
				std::stringstream ss;
				ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
				if (ImGui::Button(ss.str().c_str())) {
					selection->DeleteComponent( component );
				} else {
					ImGui::SameLine();
					DrawComponent(*component);

					/*scene::Material* mat = dynamic_cast<scene::Material*>(component);
					if (mat)
						if (ImGui::Button("Set"))
							ImGui::OpenPopup("Set Material");*/
				}

			}
			ImGui::Separator();

			if (ImGui::BeginPopup("Set Material"))
			{
// 				for (auto data : m_pEditorApplication->GetRenderer()->GetTextureData())
// 				{
// 					// Material
// 					if (ImGui::MenuItem(data.second.path.c_str()))
// 					{
// 						//Create new Component 
// 						scene::Material* mat = m_pEditorApplication->GetSelectedGameobject()->GetComponent<scene::Material>();
// 						mat->SetTextureKey(data.first);
// 						ImGui::CloseCurrentPopup();
// 					}
// 				}
				ImGui::EndPopup();
			}

			//Delete button
			if (ImGui::Button("Delete Gameobject"))
			{
				//m_pEditorApplication->GetCurrentScene()->DeleteGameobjectLater(m_pEditorApplication->GetSelectedGameobject());
				//m_pEditorApplication->SetSelectedGameobject(nullptr);
			}

			ImGui::SameLine();

			//Add component button
			if (ImGui::Button("Add component"))
				ImGui::OpenPopup("New component");

			//NewComponentPopup();

		} ImGui::End();
		SetVisible( visible );
	}

	void InspectorWindow::DrawComponent(scene::Component & _component) {
		switch (typeid(_component).hash_code())	{
			case util::TypeHash<scene::Transform>() : {
				DrawTransform(dynamic_cast<scene::Transform &>(_component));
			}break;
			default: {
				ImGui::Text("eeeeeu");
			}
			break;
		}			

	}
	void InspectorWindow::DrawTransform(scene::Transform & _transform) {

		ImGui::Text((std::string("mouais") + _transform.GetName()).c_str() );
	}
}