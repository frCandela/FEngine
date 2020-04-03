#include "game/components/fanDamage.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Damage, "damage" );

	//================================================================================================================================
	//================================================================================================================================
	void Damage::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &Damage::OnGui;
		_info.init = &Damage::Init;
		_info.load = &Damage::Load;
		_info.save = &Damage::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Init( EcsWorld& _world, Component& _component )
	{
		Damage& damage = static_cast<Damage&>( _component );
		damage.damage = 1.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		Damage& damage = static_cast<Damage&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "damage ##Damage", &damage.damage );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Save( const Component& _component, Json& _json )
	{
		const Damage& damage = static_cast<const Damage&>( _component );
		Serializable::SaveFloat( _json, "damage", damage.damage );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Load( Component& _component, const Json& _json )
	{
		Damage& damage = static_cast<Damage&>( _component );
		Serializable::LoadFloat( _json, "damage", damage.damage );
	}
}