#include "fanIncludes.h"

#include "editor/fanInspectorWindow.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "fanEngine.h"
#include "util/fanUtil.h"

namespace editor {
	InspectorWindow::InspectorWindow() {}

	void InspectorWindow::Draw() {
		if (IsVisible() == true) {
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
	}

	void InspectorWindow::DrawComponent(scene::Component & _component) {
		const std::type_info& typeInfo = typeid(_component);

		if( typeInfo == typeid(scene::Transform)){
			DrawTransform(dynamic_cast<scene::Transform &>(_component));
		} else if (typeInfo == typeid(scene::Camera)) {
			DrawCamera(dynamic_cast<scene::Camera &>(_component));
		} else {
			ImGui::Text( (std::string("Component not supported: ") + std::string(_component.GetName())).c_str());
		}
	}

	void InspectorWindow::DrawCamera(scene::Camera & _camera) {
		ImGui::Text(_camera.GetName().c_str());

		float fov = _camera.GetFov();
		if (ImGui::DragFloat("fov", &fov, 1.f, 0.f, 180.f)) {
			_camera.SetFov(fov);
		}		

		float near = _camera.GetNearDistance();
		if (ImGui::DragFloat("near distance", &near, 0.025f, 0.f, std::numeric_limits<float>::max())) {
			_camera.SetNearDistance(near);
		}		
		
		float far = _camera.GetFarDistance();
		if ( ImGui::DragFloat("far distance", &far, 1.f, 0.f, std::numeric_limits<float>::max())) {
			_camera.SetFarDistance(far);
		}
	}

	void InspectorWindow::DrawTransform(scene::Transform & _transform) {
		ImGui::Text(_transform.GetName().c_str());

		// Position
		if (ImGui::Button("##TransPos"))
			_transform.SetPosition(glm::vec3(0, 0, 0));
		ImGui::SameLine();

		float posBuffer[3] = { _transform.GetPosition().x, _transform.GetPosition().y, _transform.GetPosition().z };
		if (ImGui::DragFloat3("position", posBuffer, 0.1f))
		{
			_transform.SetPosition(glm::vec3(posBuffer[0], posBuffer[1], posBuffer[2]));
		}

		// rotation
		if (ImGui::Button("##TransRot"))
			_transform.SetRotation(glm::quat(0, 0, 0, 1));
		ImGui::SameLine();
		glm::vec3 euler = glm::eulerAngles(_transform.GetRotation());
		float angles[3] = { glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z) };
		if (ImGui::DragFloat3("rotation", angles, 0.1f))
			_transform.SetRotation(glm::quat(glm::vec3(glm::radians(angles[0]), glm::radians(angles[1]), glm::radians(angles[2]))));

		// Scale
		if (ImGui::Button("##TransScale"))
			_transform.SetScale(glm::vec3(1, 1, 1));
		ImGui::SameLine();
		float scaleBuffer[3] = { _transform.GetScale().x, _transform.GetScale().y, _transform.GetScale().z };
		if (ImGui::DragFloat3("scale", scaleBuffer, 0.1f))
			_transform.SetScale(glm::vec3(scaleBuffer[0], scaleBuffer[1], scaleBuffer[2]));
	}
}