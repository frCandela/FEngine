#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	//  processes server packets to replicates objects / run events
	//========================================================================================================
	struct ClientReplication : public EcsComponent
	{
		ECS_COMPONENT( ClientReplication )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		void ProcessPacket( PacketReplication& _packet );

		std::vector< PacketReplication > mReplicationListSingletons;
		std::vector< PacketReplication > mReplicationListEntities;
		std::vector< PacketReplication > mReplicationListRPC;
	};
}
