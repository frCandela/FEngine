#include "fanGlobalIncludes.h"
#include "scene/components/fanMaterial.h"

#include "scene/fanEntity.h"
#include "scene/components/fanModel.h"
#include "renderer/core/fanTexture.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(Material);
	REGISTER_TYPE_INFO(Material)

	Signal< Material * > Material::onMaterialAttach;
	Signal< Material * > Material::onMaterialDetach;
	Signal< Material *, std::string  > Material::onMaterialSetPath;

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
	void Material::SetShininess(const uint32_t _shininess) {
		m_shininess = _shininess;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetColor( const Color _color ) {
		m_color = _color;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui() {
		Component::OnGui();

		// Filter color
		if ( ImGui::Button( "##color" ) ) { SetColor( Color::White ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "color", m_color.Data(), gui::colorEditFlags ) ) { MarkModified(); }

		if ( ImGui::Button( "##shininess" ) ) { SetShininess( 1 ); } ImGui::SameLine();
		if( ImGui::DragInt("shininess", (int*)&m_shininess, 1, 1, 256 )) { MarkModified(); }
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
			onMaterialSetPath.Emmit( this, m_pathBuffer.string() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Save( std::ostream& _out, const int _indentLevel ) const {
		const std::string indentation = GetIndentation( _indentLevel );
		_out << indentation << "material: " << ( m_texture != nullptr ? m_texture->GetPath() : "void" ) << std::endl;
		_out << indentation << "shininess: " << m_shininess << std::endl;
		_out << indentation << "color: " << m_color[0] << " " << m_color[1] << " " << m_color[2] << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Load( std::istream& _in ) {
		std::string path;
		if ( !ReadSegmentHeader( _in, "material:" ) ) { return false; }
		if ( !ReadString( _in, path ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "shininess:" ) ) { return false; }
		if ( !ReadUnsigned( _in, m_shininess ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "color:" ) ) { return false; }
		if ( !ReadFloat3( _in, &m_color[0] ) ) { return false; }

		if ( path != std::string( "void" ) ) {
			onMaterialSetPath.Emmit( this, path );
		}
		return true;
	}
}