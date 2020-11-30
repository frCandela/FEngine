#include "project_spaceships/components/fanWeapon.hpp"

#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Weapon::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Weapon::Load;
		_info.save        = &Weapon::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Weapon::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		// bullets
		weapon.mBulletSpeed      = 0.2f;
		weapon.mBulletsPerSecond = 10.f;
		weapon.mBulletEnergyCost = 1.f;
		weapon.mOriginOffset     = btVector3::Zero();
		weapon.bBulletsTimeAccumulator = 0.f;
		weapon.mBulletPrefab.Set( nullptr );
	}
	
	//========================================================================================================
	//========================================================================================================
	void Weapon::Save( const EcsComponent& _component, Json& _json )
	{
		const Weapon& weapon = static_cast<const Weapon&>( _component );

		Serializable::SavePrefabPtr( _json, "bullet_prefab", weapon.mBulletPrefab );
		Serializable::SaveFloat( _json, "speed", weapon.mBulletSpeed );
		Serializable::SaveVec3( _json, "offset", weapon.mOriginOffset );
		Serializable::SaveFloat( _json, "bullets_per_second", weapon.mBulletsPerSecond );
		Serializable::SaveFloat( _json, "bullet_energy_cost", weapon.mBulletEnergyCost );
	}
	
	//========================================================================================================
	//========================================================================================================
	void Weapon::Load( EcsComponent& _component, const Json& _json )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		Serializable::LoadPrefabPtr( _json, "bullet_prefab", weapon.mBulletPrefab );
		Serializable::LoadFloat( _json, "speed", weapon.mBulletSpeed );
		Serializable::LoadVec3( _json, "offset", weapon.mOriginOffset );
		Serializable::LoadFloat( _json, "bullets_per_second", weapon.mBulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", weapon.mBulletEnergyCost );
	}
}