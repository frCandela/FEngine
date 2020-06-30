#include "game/components/fanDamage.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Damage::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.group = EngineGroups::Game;
		_info.onGui = &Damage::OnGui;
		_info.load = &Damage::Load;
		_info.save = &Damage::Save;
		_info.editorPath = "game/";
		_info.name = "damage";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Damage& damage = static_cast<Damage&>( _component );
		damage.damage = 1.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		Damage& damage = static_cast<Damage&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "damage ##Damage", &damage.damage );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Save( const EcsComponent& _component, Json& _json )
	{
		const Damage& damage = static_cast<const Damage&>( _component );
		Serializable::SaveFloat( _json, "damage", damage.damage );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Damage::Load( EcsComponent& _component, const Json& _json )
	{
		Damage& damage = static_cast<Damage&>( _component );
		Serializable::LoadFloat( _json, "damage", damage.damage );
	}
}