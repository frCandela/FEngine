#include "game/components/fanWeapon.hpp"

#include "editor/fanModals.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.group = EngineGroups::Game;
		_info.onGui = &Weapon::OnGui;
		_info.load = &Weapon::Load;
		_info.save = &Weapon::Save;
		_info.editorPath = "game/";
		_info.name = "weapon";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		// bullets
		weapon.bulletSpeed = 0.2f;
		weapon.bulletsPerSecond = 10.f;
		weapon.bulletEnergyCost = 1.f;
		weapon.originOffset = btVector3::Zero();

		// time accumulator
		weapon.bulletsAccumulator = 0.f;

		weapon.bulletPrefab.Set( nullptr );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanPrefab( "bullet prefab", weapon.bulletPrefab );			
			ImGui::FanToolTip("must have a transform, rigidbody, sphere shape & motion state");
			ImGui::DragFloat( "speed ##weapon", &weapon.bulletSpeed, 0.1f, 0.f, 100.f );
			ImGui::DragFloat3( "offset ##weapon", &weapon.originOffset[0] );
			ImGui::DragFloat( "bullets per second", &weapon.bulletsPerSecond, 1.f, 0.f, 1000.f );
			ImGui::DragFloat( "bullet energy cost", &weapon.bulletEnergyCost, 0.05f, 0.f, 10.f );
		} ImGui::PopItemWidth();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Save( const EcsComponent& _component, Json& _json )
	{
		const Weapon& weapon = static_cast<const Weapon&>( _component );

		Serializable::SavePrefabPtr( _json, "bullet_prefab", weapon.bulletPrefab );
		Serializable::SaveFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::SaveVec3( _json, "offset", weapon.originOffset );
		Serializable::SaveFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::SaveFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Load( EcsComponent& _component, const Json& _json )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		Serializable::LoadPrefabPtr( _json, "bullet_prefab", weapon.bulletPrefab );
		Serializable::LoadFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::LoadVec3( _json, "offset", weapon.originOffset );
		Serializable::LoadFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
	}
}