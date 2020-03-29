#include "scene/components/fanMaterial.hpp"

#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	REGISTER_COMPONENT( Material, "material" );

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MATERIAL16;
		_info.onGui = &Material::OnGui;
		_info.init = &Material::Init;
		_info.load  = &Material::Load;
		_info.save  = &Material::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Init( EcsWorld& _world, Component& _component )
	{
		Material& material = static_cast<Material&>( _component );

		material.texture = nullptr;
		material.shininess = 1;
		material.color = Color::White;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		Material& material = static_cast<Material&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// Filter color
			if( ImGui::Button( "##color" ) ) { material.color = Color::White; } ImGui::SameLine();
			ImGui::ColorEdit4( "color", material.color.Data(), ImGui::fanColorEditFlags );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags()& Gameobject::Flag::OUTDATED_MATERIAL ); }

			if( ImGui::Button( "##shininess" ) ) { material.shininess = 1; } ImGui::SameLine();
			ImGui::DragInt( "shininess", (int*)&material.shininess, 1, 1, 256 );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_MATERIAL ); }
			ImGui::SameLine(); ImGui::FanShowHelpMarker( "sharpness of the specular reflection" );
		} ImGui::PopItemWidth();

		ImGui::FanTexturePtr( "mat texture", material.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Save( const Component& _component, Json& _json )
	{
		const Material& material = static_cast<const Material&>( _component );

		Serializable::SaveUInt( _json, "shininess", material.shininess );
		Serializable::SaveColor( _json, "color", material.color );
		Serializable::SaveTexturePtr( _json, "material", material.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Load( Component& _component, const Json& _json )
	{
		Material& material = static_cast<Material&>( _component );

		Serializable::LoadUInt( _json, "shininess", material.shininess );
		Serializable::LoadColor( _json, "color", material.color );
		Serializable::LoadTexturePtr( _json, "material", material.texture );
	}
}