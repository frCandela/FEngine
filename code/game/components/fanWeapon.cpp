#include "game/components/fanWeapon.hpp"

// #include "game/components/fanWithEnergy.hpp"
// #include "game/components/fanPlayerInput.hpp"
// #include "core/input/fanInput.hpp"
// #include "core/input/fanInputManager.hpp"
// #include "core/time/fanProfiler.hpp"
// #include "core/input/fanMouse.hpp"
// #include "core/time/fanTime.hpp"
#include "editor/fanModals.hpp"
#include "game/fanGameSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Weapon, "weapon" );

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &Weapon::OnGui;
		_info.init = &Weapon::Init;
		_info.load = &Weapon::Load;
		_info.save = &Weapon::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Init( EcsWorld&, Component& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		// bullets
		weapon.bulletSpeed = 0.2f;
		weapon.bulletsPerSecond = 10.f;
		weapon.bulletEnergyCost = 1.f;
		weapon.originOffset = btVector3::Zero();

		// explosion parameters
		weapon.explosionTime = 0.15f;
		weapon.exposionSpeed = 2.f;
		weapon.particlesPerExplosion = 3;

		// time accumulator
		weapon.bulletsAccumulator = 0.f;

		weapon.bulletPrefab = nullptr;		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui( Component& _component )
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
			ImGui::Spacing(); 
			ImGui::DragFloat( "explosion time ##weapon", &weapon.explosionTime, 0.05f, 0.f, 10.f );
			ImGui::DragFloat( "explosion speed ##weapon", &weapon.exposionSpeed, 0.5f, 0.f, 100.f );
			ImGui::DragInt( "particles per explosion ##weapon", &weapon.particlesPerExplosion );
		} ImGui::PopItemWidth();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Save( const Component& _component, Json& _json )
	{
		const Weapon& weapon = static_cast<const Weapon&>( _component );

		Serializable::SavePrefabPtr( _json, "bullet_prefab", weapon.bulletPrefab );
		Serializable::SaveFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::SaveVec3( _json, "offset", weapon.originOffset );
		Serializable::SaveFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::SaveFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
		Serializable::SaveFloat( _json, "exposion_speed", weapon.exposionSpeed );
		Serializable::SaveFloat( _json, "explosion_time", weapon.explosionTime );
		Serializable::SaveInt( _json, "particlesPerExplosion", weapon.particlesPerExplosion );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Load( Component& _component, const Json& _json )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		Serializable::LoadPrefabPtr( _json, "bullet_prefab", weapon.bulletPrefab );
		Serializable::LoadFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::LoadVec3( _json, "offset", weapon.originOffset );
		Serializable::LoadFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
		Serializable::LoadFloat( _json, "exposion_speed", weapon.exposionSpeed );
		Serializable::LoadFloat( _json, "explosion_time", weapon.explosionTime );
		Serializable::LoadInt( _json, "particlesPerExplosion", weapon.particlesPerExplosion );
	}
}