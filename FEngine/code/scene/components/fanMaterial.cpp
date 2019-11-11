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
	void Material::SetTexture( Texture * const _texture )
	{
		if ( _texture != nullptr ) 
		{ 
			m_material->texture = TexturePtr( _texture, _texture->GetPath() ); 
		}
		else
		{
			m_material->texture = TexturePtr(); 
		}
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
	Texture *		Material::GetTexture()			{ return *m_material->texture; }
	const Texture *	Material::GetTexture() const	{ return *m_material->texture; }
	const uint32_t	Material::GetShininess() const	{ return m_material->shininess; }
	Color			Material::GetColor() const		{ return m_material->color; }

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui() {
		Component::OnGui();

		// Filter color
		if ( ImGui::Button( "##color" ) ) { SetColor( Color::White ); } ImGui::SameLine();
		if ( ImGui::ColorEdit4( "color", m_material->color.Data(), ImGui::fanColorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }

		if ( ImGui::Button( "##shininess" ) ) { SetShininess( 1 ); } ImGui::SameLine();
		if( ImGui::DragInt("shininess", (int*)&m_material->shininess, 1, 1, 256 )) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_MATERIAL ); }
		ImGui::SameLine(); ImGui ::FanShowHelpMarker("sharpness of the specular reflection");

		ImGui::FanTexture("mat texture",& m_material->texture);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexturePath( const std::string& _path ) {
		m_material->texture.InitUnresolved(_path);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Load( Json & _json ) {
		LoadUInt  ( _json, "shininess", m_material->shininess );
		LoadColor ( _json, "color", m_material->color );
		LoadTexturePtr(_json, "material", m_material->texture );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Material::Save( Json & _json ) const {
				SaveUInt  ( _json, "shininess", m_material->shininess );
		SaveColor ( _json, "color", m_material->color );
		SaveTexturePtr(_json, "material", m_material->texture );
		Component::Save( _json );		

		return true;
	}
}