#pragma once

#include "ecs/fanEcsComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;
	struct ClientRPC;

	//================================================================================================================================
	//  processes server packets to replicates objects / run events
	//================================================================================================================================
	struct ClientReplication : public EcsComponent
	{
		ECS_COMPONENT( ClientReplication )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		void ProcessPacket( PacketReplication& _packet );

		std::vector< PacketReplication > replicationListSingletons;
		std::vector< PacketReplication > replicationListEntities;
		std::vector< PacketReplication > replicationListRPC;
	};
}
