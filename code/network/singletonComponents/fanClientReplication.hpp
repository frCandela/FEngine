#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;
	struct RPCManager;

	//================================================================================================================================
	//  processes server packets to replicates objects / run events
	//================================================================================================================================
	struct ClientReplication : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		void ProcessPacket( PacketReplication& _packet );
		void ReplicateRPC( RPCManager& _rpcManager );
		void ReplicateSingletons( EcsWorld& _world );

		std::vector< PacketReplication > replicationListSingletons;
		std::vector< PacketReplication > replicationListRPC;
	};
}
