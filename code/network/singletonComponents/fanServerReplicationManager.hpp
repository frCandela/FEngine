#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// [Server] sends packets to clients to replicates objects / run PRC
	//================================================================================================================================
	struct ServerReplicationManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		enum ReplicationFlags
		{
			None = 0,
			ResendUntilReplicated = 1 << 1	// if the packets is droppeds, resend it
		};

		//================================================================
		//================================================================
		struct SingletonReplicationData
		{
			ReplicationFlags flags = ReplicationFlags::None;	// replication parameters
			PacketReplication packet;							// saved replication data
		};

		//================================================================
		//================================================================
		struct HostData
		{
			bool isNull = false;													// client was deleted
			std::multimap< PacketTag, SingletonReplicationData> pendingReplication; // sent on the network, waiting for a status
			std::vector<SingletonReplicationData>				nextReplication;	// waiting  to be sent on the network	
		};

		std::vector<HostData> hostDatas;

		void CreateHost( const HostID _hostID );
		void DeleteHost( const HostID _hostID );
		void Send( Packet& _packet, const HostID _hostID );
		void ReplicateOnClient( const HostID _hostID, PacketReplication& _packet, const ReplicationFlags _flags );
		void ReplicateOnAllClients( PacketReplication& _packet, const ReplicationFlags _flags );

		static PacketReplication BuildSingletonPacket( const EcsWorld& _world, const uint32_t _staticID );
		static PacketReplication BuildRPCPacket( sf::Packet& _dataRPC );

		void OnReplicationSuccess( const HostID _hostID, const PacketTag _packetTag );
		void OnReplicationFail( const HostID _hostID, const PacketTag _packetTag );


	};
}
