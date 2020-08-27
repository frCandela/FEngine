#pragma once

#include "bullet/LinearMath/btVector3.h"
#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	// fires bullets pew pew
	// is placed on the spaceship on the player 
	//========================================================================================================
	struct Weapon : public EcsComponent
	{
		ECS_COMPONENT( Weapon )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		// Bullets parameters
		PrefabPtr mBulletPrefab;
		btVector3 mOriginOffset;
		float     mBulletSpeed;
		float     mBulletsPerSecond;
		float     mBulletEnergyCost;
		float     bBulletsTimeAccumulator;
	};
}