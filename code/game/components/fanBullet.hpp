#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	// A projectile that deals damage 
	// @todo merge this with the damage component
	//================================================================================================================================
	struct Bullet : public EcsComponent
	{
		ECS_COMPONENT( Bullet )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

// 		Bullet::Bullet( const Bullet& _other ) :
// 			explosionPrefab( _other.explosionPrefab )
// 		{
// 		}

		PrefabPtr explosionPrefab;
	};
	static constexpr size_t sizeof_bullet = sizeof( Bullet );
}