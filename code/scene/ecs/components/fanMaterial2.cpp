#include "scene/ecs/components/fanMaterial2.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Material2, "material" );

	//================================================================================================================================
	//================================================================================================================================
	void Material2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MATERIAL16;
		_info.onGui = &Material2::OnGui;
		_info.clear = &Material2::Clear;
		_info.load  = &Material2::Load;
		_info.save  = &Material2::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material2::Clear( ecComponent& _material )
	{
		Material2& material = static_cast<Material2&>( _material );

		material.texture = nullptr;
		material.shininess = 1;
		material.color = Color::White;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material2::OnGui( ecComponent& _material )
	{
		Material2& material = static_cast<Material2&>( _material );

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
	void Material2::Save( const ecComponent& _material, Json& _json )
	{
		const Material2& material = static_cast<const Material2&>( _material );

		Serializable::SaveUInt( _json, "shininess", material.shininess );
		Serializable::SaveColor( _json, "color", material.color );
		Serializable::SaveTexturePtr( _json, "material", material.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material2::Load( ecComponent& _material, const Json& _json )
	{
		Material2& material = static_cast<Material2&>( _material );

		Serializable::LoadUInt( _json, "shininess", material.shininess );
		Serializable::LoadColor( _json, "color", material.color );
		Serializable::LoadTexturePtr( _json, "material", material.texture );
	}
}