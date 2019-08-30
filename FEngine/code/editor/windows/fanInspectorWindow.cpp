#include "fanIncludes.h"

#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanMaterial.h"
#include "editor/components/fanFPSCamera.h"
#include "editor/fanModals.h"
#include "core/ressources/fanMesh.h"
#include "core/fanSignal.h"
#include "vulkan/core/vkTexture.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/core/vkTexture.h"
#include "vulkan/fanTexturesManager.h"
#include "fanEngine.h"

namespace editor {
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() {
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::Draw() {
		if (IsVisible() == true) {
			fan::Engine & engine = fan::Engine::GetEngine();
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
					scene::Component * component =  components[componentIndex];

					ImGui::Separator();

					// Delete button

					if ( component->IsRemovable()) {
						std::stringstream ss;
						ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
						if (ImGui::Button(ss.str().c_str())) {
							selection->DeleteComponent(component);
							component = nullptr;
						} ImGui::SameLine();
					}
					if(component != nullptr){
						DrawComponent(*component);
					}

						/*scene::Material* mat = dynamic_cast<scene::Material*>(component);
						if (mat)
							if (ImGui::Button("Set"))
								ImGui::OpenPopup("Set Material");*/
					

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

				ImGui::SameLine();

				//Add component button
				if (ImGui::Button("Add component"))
					ImGui::OpenPopup("New component");

				NewComponentPopup();

			} ImGui::End();
			SetVisible( visible );
		}		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup() {

		if (ImGui::BeginPopup("New component"))
		{
			fan::Engine & engine = fan::Engine::GetEngine();
			scene::Gameobject * const selection = engine.GetSelectedGameobject();

			// Mesh
			if (ImGui::MenuItem("Model")){
				selection->AddComponent<scene::Model>();
				ImGui::CloseCurrentPopup();
			}

			// Camera
			if (ImGui::MenuItem("Camera")) {
				// Create new Component 
				selection->AddComponent<scene::Camera>();
				ImGui::CloseCurrentPopup();
			}

			// Material
			/*if (ImGui::MenuItem("Material"))
			{
				// Create new Component 
				scene::Material* mat = m_pEditorApplication->GetSelectedGameobject()->AddComponent<editor::Material>();
				mat->SetTextureKey(m_pEditorApplication->GetRenderer()->GetDefaultTexture());
				ImGui::CloseCurrentPopup();
			}*/

			// Material
			/*if (ImGui::MenuItem("RigidBody"))
			{
				// Create new Component 
				m_pEditorApplication->GetSelectedGameobject()->AddComponent<editor::Rigidbody>();
				ImGui::CloseCurrentPopup();
			}*/

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawComponent(scene::Component & _component) {
		if(_component.IsType<scene::Transform>()){
			DrawTransform(static_cast<scene::Transform &>(_component));
		} else if (_component.IsType < scene::Camera>()) {
			DrawCamera(static_cast<scene::Camera &>(_component));
		} else if (_component.IsType < scene::Model > ()) {
			DrawModel(static_cast<scene::Model &>(_component));
		} else if (_component.IsType < scene::FPSCamera>()) {
			DrawFPSCamera(static_cast<scene::FPSCamera &>(_component));
		} else if (_component.IsType < scene::Material>()) {
			DrawMaterial(static_cast<scene::Material &>(_component));
		}else {
			ImGui::Text( (std::string("Component not supported: ") + std::string(_component.GetName())).c_str());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawCamera(scene::Camera & _camera) {
		ImGui::Text(_camera.GetName());

		// fov
		if (ImGui::Button("##fov")) {
			_camera.SetFov(110.f);
		}
		ImGui::SameLine();
		float fov = _camera.GetFov();
		if (ImGui::DragFloat("fov", &fov, 1.f, 1.f, 179.f)) {
			_camera.SetFov(fov);
		}	

		// nearDistance
		if (ImGui::Button("##nearDistance")) {
			_camera.SetNearDistance(0.01f);
		}
		ImGui::SameLine();
		float near = _camera.GetNearDistance();
		if (ImGui::DragFloat("near distance", &near, 0.001f, 0.01f, 10.f)) {
			_camera.SetNearDistance(near);
		}		
		
		// far distance
		if (ImGui::Button("##fardistance")) {
			_camera.SetFarDistance(1000.f);
		}
		ImGui::SameLine();
		float far = _camera.GetFarDistance();
		if ( ImGui::DragFloat("far distance", &far, 10.f, 0.05f, 10000.f)) {
			_camera.SetFarDistance(far);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawTransform(scene::Transform & _transform) {
		ImGui::Text(_transform.GetName());

		// Position
		if (ImGui::Button("##TransPos")) {
			_transform.SetPosition(btVector3(0, 0, 0));
		} ImGui::SameLine();
		float posBuffer[3] = { _transform.GetPosition().x(), _transform.GetPosition().y(), _transform.GetPosition().z() };
		if (ImGui::DragFloat3("position", posBuffer, 0.1f)) {
			_transform.SetPosition(btVector3(posBuffer[0], posBuffer[1], posBuffer[2]));
		}

		// rotation
		if (ImGui::Button("##TransRot")) {
			_transform.SetRotationEuler(btVector3(0, 0, 0));
		} ImGui::SameLine(); 	
		const btVector3 rot = _transform.GetRotationEuler();
		float bufferAngles[3] = { rot.x(),rot.y(),rot.z()};
		if (ImGui::DragFloat3("rotation", bufferAngles, 0.1f))	{
			_transform.SetRotationEuler(btVector3(bufferAngles[0], bufferAngles[1], bufferAngles[2]));
		}

		// Scale
		if (ImGui::Button("##TransScale")) {
			_transform.SetScale(btVector3(1, 1, 1));
		} ImGui::SameLine();
		btVector3 scale = _transform.GetScale();
		if (ImGui::DragFloat3("scale", &scale[0], 0.1f)) {
			_transform.SetScale(scale);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawModel(scene::Model & _model) {
		ImGui::Text(_model.GetName());
		// Set path popup
		bool openSetPathPopup = false;
		if (ImGui::Button("##setPath")) {
			openSetPathPopup = true;
		}
		ImGui::SameLine();
		const std::string meshPath = _model.GetMesh() != nullptr ? _model.GetMesh()->GetPath() : "not set";
		ImGui::Text("path: %s", meshPath.c_str());
		// Set path  popup on double click
		if (openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			if (_model.GetMesh() != nullptr && _model.GetMesh()->GetPath().empty() == false) {
				m_pathBuffer = std::fs::path(_model.GetMesh()->GetPath()).parent_path();
			}
			else {
				m_pathBuffer = "./";
			}
			ImGui::OpenPopup("set_path");
			m_pathBuffer = "content/models";
		}

		if (util::Imgui::LoadFileModal("set_path", { ".fbx" }, m_pathBuffer)) {

			ressource::Mesh * mesh = vk::Renderer::GetRenderer().FindMesh(DSID(m_pathBuffer.string().c_str()));			
			if (mesh == nullptr) {
				mesh = new ressource::Mesh(m_pathBuffer.string());
				mesh->Load();
				_model.SetMesh(mesh);
				
			} else {
				_model.SetMesh(mesh);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawFPSCamera(scene::FPSCamera & _fpsCamera) {
		ImGui::Text(_fpsCamera.GetName());



		// SetSensitivity
		if (ImGui::Button("##SetSensitivity")) {
			_fpsCamera.SetXYSensitivity(btVector2(0.005f, 0.005f) );
		} ImGui::SameLine();
		btVector2 xySensitivity = _fpsCamera.GetXYSensitivity();
		if (ImGui::DragFloat2("XY sensitivity", &xySensitivity[0], 1.f)) {
			_fpsCamera.SetXYSensitivity(xySensitivity);
		}

		// SetSpeed
		if (ImGui::Button("##SetSpeed")) {
			_fpsCamera.SetSpeed(10.f);
		} ImGui::SameLine();
		float speed = _fpsCamera.GetSpeed();
		if (ImGui::DragFloat("speed", &speed, 1.f)) {
			_fpsCamera.SetSpeed(speed);
		}

		// SetSpeedMultiplier
		if (ImGui::Button("##SetSpeedMultiplier")) {
			_fpsCamera.SetSpeedMultiplier(3.f);
		} ImGui::SameLine();
		float speedMultiplier = _fpsCamera.GetSpeedMultiplier();
		if (ImGui::DragFloat("speed multiplier", &speedMultiplier, 1.f)) {
			_fpsCamera.SetSpeedMultiplier(speedMultiplier);
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawMaterial(scene::Material & _material) {
		ImGui::Text(_material.GetName());

		bool openSetPathPopup = false;
		if (ImGui::Button("##setPathTex")) {
			openSetPathPopup = true;
		}
		ImGui::SameLine();
		const std::string texturePath = _material.GetTexture() != nullptr ? _material.GetTexture()->GetPath() : "not set";
		ImGui::Text("texture : %s", texturePath.c_str());
		// Set path  popup on double click
		if (openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			if (_material.GetTexture() != nullptr && _material.GetTexture()->GetPath().empty() == false) {
				m_pathBuffer = std::fs::path(_material.GetTexture()->GetPath()).parent_path();
			}
			else {
				m_pathBuffer = "./";
			}
			ImGui::OpenPopup("set_path_texture");
			m_pathBuffer = "content/models";
		}

		if (util::Imgui::LoadFileModal("set_path_texture", { ".png" }, m_pathBuffer)) {
			vk::TexturesManager * texturesManager = vk::Renderer::GetRenderer().GetTexturesManager();
			vk::Texture * texture = texturesManager->FindTexture(m_pathBuffer.string());
			if (texture == nullptr) {
				texture = texturesManager->LoadTexture( m_pathBuffer.string() );
			} 
			_material.SetTexture(texture);
		}
	
	}
}