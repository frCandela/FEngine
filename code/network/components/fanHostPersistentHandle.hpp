#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// Contains the handle of the player network persistent entity
	// Is placed on the player spaceship
	//================================================================================================================================	
	struct HostPersistentHandle : public EcsComponent
	{
		ECS_COMPONENT( HostPersistentHandle )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		EcsHandle handle;
	};
}