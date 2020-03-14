#include "scene/ecs/components/fanDirectionalLight2.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( DirectionalLight2, "directional_light" );

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::DIR_LIGHT16;
		_info.onGui = &DirectionalLight2::OnGui;
		_info.init = &DirectionalLight2::Init;
		_info.load = &DirectionalLight2::Load;
		_info.save = &DirectionalLight2::Save;
		_info.editorPath = "lights/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight2::Init( ecComponent& _component )
	{
		DirectionalLight2& light = static_cast<DirectionalLight2&>( _component );
		light.ambiant = Color::Black;
		light.diffuse = Color::White;
		light.specular = Color::White;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight2::OnGui( ecComponent& _light )
	{
		DirectionalLight2& light = static_cast<DirectionalLight2&>( _light );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Filter color
			if( ImGui::Button( "##ambiant" ) ) { light.ambiant = Color::Black; } ImGui::SameLine();
			ImGui::ColorEdit3( "ambiant", light.ambiant.Data(), ImGui::fanColorEditFlags ); // ) { m_gameobject->SetFlags( m_gameobject->GetFlags()& Gameobject::Flag::OUTDATED_LIGHT ); }
			
			if( ImGui::Button( "##diffuse" ) ) { light.diffuse = Color::Black; } ImGui::SameLine();
			ImGui::ColorEdit3( "diffuse", light.diffuse.Data(), ImGui::fanColorEditFlags ); // ) { m_gameobject->SetFlags( m_gameobject->GetFlags()& Gameobject::Flag::OUTDATED_LIGHT ); }
			
			if( ImGui::Button( "##specular" ) ) { light.specular = Color::Black; } ImGui::SameLine();
			ImGui::ColorEdit3( "specular", light.specular.Data(), ImGui::fanColorEditFlags ); // ) { m_gameobject->SetFlags( m_gameobject->GetFlags()& Gameobject::Flag::OUTDATED_LIGHT ); }


// 			const Transform& transform = m_gameobject->GetTransform();
// 			const btVector3 pos = transform.GetPosition();
// 			const btVector3 dir = transform.Forward();
// 			const btVector3 up = transform.Up();
// 			const btVector3 left = transform.Left();
// 			const float length = 2.f;
// 			const float radius = 0.5f;
// 			const Color color = Color::Yellow;
// 			btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
// 			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
// 			{
// 				const btVector3 offset = offsets[offsetIndex];
// 				RendererDebug::Get().DebugLine( pos + offset, pos + offset + length * dir, color );
// 			}
// 			RendererDebug::Get().DebugSphere( transform.GetBtTransform(), radius, 0, color );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight2::Save( const ecComponent& _light, Json& _json )
	{
		const DirectionalLight2& light = static_cast<const DirectionalLight2&>( _light );
		Serializable::SaveColor( _json, "ambiant", light.ambiant );
		Serializable::SaveColor( _json, "diffuse", light.diffuse );
		Serializable::SaveColor( _json, "specular", light.specular );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight2::Load( ecComponent& _light, const Json& _json )
	{
		DirectionalLight2& light = static_cast<DirectionalLight2&>( _light );
		Serializable::LoadColor( _json, "ambiant", light.ambiant );
		Serializable::LoadColor( _json, "diffuse", light.diffuse );
		Serializable::LoadColor( _json, "specular", light.specular );
	}
}