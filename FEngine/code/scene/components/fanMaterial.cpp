#include "fanGlobalIncludes.h"
#include "scene/components/fanMaterial.h"

#include "scene/fanEntity.h"
#include "scene/components/fanModel.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(Material);
	REGISTER_TYPE_INFO(Material)

		Signal< Material * > Material::onMaterialAttach;
	Signal< Material * > Material::onMaterialDetach;

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnDetach() {
		Component::OnDetach();
		onMaterialDetach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnAttach() {
		Component::OnAttach();
		onMaterialAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(Texture * const _texture) {
		m_texture = _texture;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetShininess(const int _shininess) {
		m_shininess = _shininess;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui() {
		Component::OnGui();

		if( ImGui::DragInt("shininess", &m_shininess, 1, 1, 256 )) { MarkModified(); }
		ImGui::SameLine(); gui ::ShowHelpMarker("sharpness of the specular reflection");

		bool openSetPathPopup = false;
		if (ImGui::Button("##setPathTex")) {
			openSetPathPopup = true;
		}
		ImGui::SameLine();
		const std::string texturePath = GetTexture() != nullptr ? GetTexture()->GetPath() : "not set";
		ImGui::Text("texture : %s", texturePath.c_str());
		// Set path  popup on double click
		if (openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			if (GetTexture() != nullptr && GetTexture()->GetPath().empty() == false) {
				m_pathBuffer = std::fs::path(GetTexture()->GetPath()).parent_path();
			}
			else {
				m_pathBuffer = "./";
			}
			ImGui::OpenPopup("set_path_texture");
			m_pathBuffer = "content/models";
		}

		if (gui::LoadFileModal("set_path_texture", GlobalValues::s_imagesExtensions, m_pathBuffer)) {
			RessourceManager * texturesManager = Renderer::Get().GetRessourceManager();
			Texture * texture = texturesManager->FindTexture(m_pathBuffer.string());
			if (texture == nullptr) {
				texture = texturesManager->LoadTexture(m_pathBuffer.string());
			}
			SetTexture(texture);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "material: " << (m_texture != nullptr ? m_texture->GetPath() : "void") << std::endl;
		_out << indentation << "shininess: " << m_shininess << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Load(std::istream& _in) {
		std::string path;
		if (!ReadSegmentHeader(_in, "material:")) { return false; }
		if (!ReadString(_in, path)) { return false; }
		if (!ReadSegmentHeader(_in, "shininess:")) { return false; }
		if (!ReadInteger(_in, m_shininess)) { return false; }

		if (path != std::string("void")) {
			// TODO find a cleaner way to set the texture
			RessourceManager * texturesManager = Renderer::Get().GetRessourceManager();
			Texture * texture = texturesManager->FindTexture(path);
			if (texture == nullptr) {
				texture = texturesManager->LoadTexture(path);
			}
			SetTexture(texture);
		}
		return true;
	}
}