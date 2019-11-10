#include "fanGlobalIncludes.h"
#include "scene/components/fanMaterial.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanMeshRenderer.h"
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
	void Material::OnAttach() {
		Component::OnAttach();

		ecsMaterial ** tmpMat = &const_cast<ecsMaterial*>( m_material );
		*tmpMat = m_gameobject->AddEcsComponent<ecsMaterial>();
		m_material->Init();

		onMaterialAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsMaterial>();
		onMaterialDetach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(Texture * const _texture) {
		m_material->texture = _texture;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetShininess(const uint32_t _shininess) {
		m_material->shininess = _shininess;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetColor( const Color _color ) {
		m_material->color = _color;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture *		Material::GetTexture()			{ return m_material->texture; }
	const Texture *	Material::GetTexture() const	{ return m_material->texture; }
	const uint32_t	Material::GetShininess() const	{ return m_material->shininess; }
	Color			Material::GetColor() const		{ return m_material->color; }

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui() {
		Component::OnGui();

		// Filter color
		if ( ImGui::Button( "##color" ) ) { SetColor( Color::White ); } ImGui::SameLine();
		if ( ImGui::ColorEdit4( "color", m_material->color.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }

		if ( ImGui::Button( "##shininess" ) ) { SetShininess( 1 ); } ImGui::SameLine();
		if( ImGui::DragInt("shininess", (int*)&m_material->shininess, 1, 1, 256 )) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }
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
			SetTexturePath( m_pathBuffer.string() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexturePath( const std::string& _path ) {
		onMaterialSetPath.Emmit( this, _path );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Load( Json & _json ) {
		LoadUInt  ( _json, "shininess", m_material->shininess );
		LoadColor ( _json, "color", m_material->color );
		std::string tmpPath;
		if ( LoadString( _json, "material", tmpPath ) && tmpPath != std::string( "void" ) ) {
			onMaterialSetPath.Emmit( this, tmpPath );
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Save( Json & _json ) const {

		SaveString( _json, "material", ( m_material->texture != nullptr ? m_material->texture->GetPath() : "void" ) );
		SaveUInt  ( _json, "shininess", m_material->shininess );
		SaveColor ( _json, "color", m_material->color );
		Component::Save( _json );		

		return true;
	}
}