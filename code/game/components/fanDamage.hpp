#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	// A damage component is used to remove health points when colliding with stuff
	//========================================================================================================
	struct Damage : public EcsComponent
	{
		ECS_COMPONENT( struct Damage )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float mDamage;
	};
}