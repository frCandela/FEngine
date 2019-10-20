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
	void Material::OnAttach() {
		Component::OnAttach();
		m_gameobject->AddEcsComponent<ecsMaterial>()->Init();
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
		GetEcsMaterial()->texture = _texture;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetShininess(const uint32_t _shininess) {
		GetEcsMaterial()->shininess = _shininess;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetColor( const Color _color ) {
		GetEcsMaterial()->color = _color;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL );
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
		if ( ImGui::ColorEdit3( "color", material->color.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }

		if ( ImGui::Button( "##shininess" ) ) { SetShininess( 1 ); } ImGui::SameLine();
		if( ImGui::DragInt("shininess", (int*)&material->shininess, 1, 1, 256 )) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }
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
	bool Material::Load( Json & _json ) {
		ecsMaterial * material = GetEcsMaterial();		

		LoadUInt  ( _json, "shininess", material->shininess );
		LoadColor ( _json, "color", material->color );
		std::string tmpPath;
		if ( LoadString( _json, "material", tmpPath ) && tmpPath != std::string( "void" ) ) {
			onMaterialSetPath.Emmit( this, tmpPath );
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Save( Json & _json ) const {
		ecsMaterial * material = GetEcsMaterial();

		SaveString( _json, "material", ( material->texture != nullptr ? material->texture->GetPath() : "void" ) );
		SaveUInt  ( _json, "shininess", material->shininess );
		SaveColor ( _json, "color", material->color );
		Component::Save( _json );		

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsMaterial* Material::GetEcsMaterial() const { return m_gameobject->GetEcsComponent<ecsMaterial>(); }
}