#include "fanBullet.hpp"

namespace fan
{

	//========================================================================================================
	//========================================================================================================
	void Bullet::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Bullet::Load;
		_info.save        = &Bullet::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		// bullets
		bullet.mExplosionPrefab.Set( nullptr );
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Save( const EcsComponent& _component, Json& _json )
	{
		const Bullet& bullet = static_cast<const Bullet&>( _component );
		Serializable::SavePrefabPtr( _json, "explosion_prefab", bullet.mExplosionPrefab );
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Load( EcsComponent& _component, const Json& _json )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );
		Serializable::LoadPrefabPtr( _json, "explosion_prefab", bullet.mExplosionPrefab );
	}
}