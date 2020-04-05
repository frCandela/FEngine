#include "game/components/fanHealth.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{	
	REGISTER_COMPONENT( Health, "health" );

	//================================================================================================================================
	//================================================================================================================================
	void Health::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::HEART16;
		_info.onGui = &Health::OnGui;
		_info.init = &Health::Init;
		_info.load = &Health::Load;
		_info.save = &Health::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::Init( EcsWorld& _world, Component& _component )
	{
		Health& health = static_cast<Health&>( _component );
		health.invincible = false;
		health.maxHealth = 100.f;
		health.currentHealth = health.maxHealth;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		Health& health = static_cast<Health&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::SliderFloat( "current health", &health.currentHealth, 0.f, health.maxHealth );
			ImGui::DragFloat( "max health", &health.maxHealth );
			ImGui::Checkbox( "invincible", &health.invincible );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::Save( const Component& _component, Json& _json )
	{
		const Health& health = static_cast<const Health&>( _component );
		Serializable::SaveFloat( _json, "max_energy", health.maxHealth );

	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::Load( Component& _component, const Json& _json )
	{
		Health& health = static_cast<Health&>( _component );
		Serializable::LoadFloat( _json, "max_energy", health.maxHealth );

	}
}