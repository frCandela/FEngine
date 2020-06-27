#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// All entities that have an EntityReplication component will be saved in a packet using NetSave / NetLoad methods.
	// Then, data will be replicated on other hosts
	//================================================================================================================================	
	struct EntityReplication : public EcsComponent
	{
		ECS_COMPONENT( EntityReplication )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		std::vector<uint32_t> componentTypes;
		EcsHandle exclude; // host handle to exclude for replication
	};
}