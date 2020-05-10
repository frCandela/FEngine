#pragma once

#include "ecs/fanComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;
	struct ClientRPC;

	//================================================================================================================================
	//  processes server packets to replicates objects / run events
	//================================================================================================================================
	struct ClientReplication : public Component
	{
		DECLARE_COMPONENT( ClientReplication )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );

		void ProcessPacket( PacketReplication& _packet );

		std::vector< PacketReplication > replicationListSingletons;
		std::vector< PacketReplication > replicationListRPC;
	};
}
