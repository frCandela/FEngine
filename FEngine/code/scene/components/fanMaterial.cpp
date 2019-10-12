#include "fanGlobalIncludes.h"
#include "scene/components/fanMaterial.h"

#include "scene/fanGameobject.h"
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
		GetGameobject()->AddEcsComponent<ecsMaterial>();
		onMaterialAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(Texture * const _texture) {
		GetEcsMaterial()->texture = _texture;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetShininess(const uint32_t _shininess) {
		GetEcsMaterial()->shininess = _shininess;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetColor( const Color _color ) {
		GetEcsMaterial()->color = _color;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture *		Material::GetTexture()			{ return GetEcsMaterial()->texture; }
	const Texture *	Material::GetTexture() const	{ return GetEcsMaterial()->texture; }
	const uint32_t	Material::GetShininess() const	{ return GetEcsMaterial()->shininess; }
	Color			Material::GetColor() const		{ return GetEcsMaterial()->color; }

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui() {
		Component::OnGui();

		ecsMaterial * material = GetEcsMaterial();

		// Filter color
		if ( ImGui::Button( "##color" ) ) { SetColor( Color::White ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "color", material->color.Data(), gui::colorEditFlags ) ) { MarkModified(); }

		if ( ImGui::Button( "##shininess" ) ) { SetShininess( 1 ); } ImGui::SameLine();
		if( ImGui::DragInt("shininess", (int*)&material->shininess, 1, 1, 256 )) { MarkModified(); }
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
		ecsMaterial * material = GetEcsMaterial();
		const std::string indentation = GetIndentation( _indentLevel );
		_out << indentation << "material: " << ( material->texture != nullptr ? material->texture->GetPath() : "void" ) << std::endl;
		_out << indentation << "shininess: " << material->shininess << std::endl;
		_out << indentation << "color: " << material->color[0] << " " << material->color[1] << " " << material->color[2] << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Load( std::istream& _in ) {
		ecsMaterial * material = GetEcsMaterial();
		std::string path;
		if ( !ReadSegmentHeader( _in, "material:" ) ) { return false; }
		if ( !ReadString( _in, path ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "shininess:" ) ) { return false; }
		if ( !ReadUnsigned( _in, material->shininess ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "color:" ) ) { return false; }
		if ( !ReadFloat3( _in, &material->color[0] ) ) { return false; }

		if ( path != std::string( "void" ) ) {
			onMaterialSetPath.Emmit( this, path );
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsMaterial* Material::GetEcsMaterial() const { return GetGameobject()->GetEcsComponent<ecsMaterial>(); }
}