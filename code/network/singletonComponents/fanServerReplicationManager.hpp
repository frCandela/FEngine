#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// [Server] sends packets to clients to replicates objects / run events
	//================================================================================================================================
	class ServerReplicationManager : public SingletonComponent
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
			EnsureReplicated = 1 << 1
		};

		//================================================================
		//================================================================
		struct SingletonReplicationData
		{
			Signal< HostID, PacketTag > onFailure;				// fail callback
			Signal< HostID, PacketTag > onSuccess;				// success callback
			uint32_t staticID;									// singleton static id
			ReplicationFlags flags = ReplicationFlags::None;	// replication parameters
			PacketReplicationSingletonComponents packet;		// saved replication data
		};

		//================================================================
		//================================================================
		struct HostData
		{
			bool isNull = false;													// client was deleted
			std::multimap< PacketTag, SingletonReplicationData> pendingReplication; // sent on the network, waiting for a status
			std::vector<SingletonReplicationData>				nextReplication;	// waiting  to be sent on the network	
		};

		void CreateHost( const HostID _hostID );
		void DeleteHost( const HostID _hostID );
		void ReplicateSingleton( const uint32_t _staticID, const ReplicationFlags _flags = ReplicationFlags::None );
		void Send( Packet& _packet, const HostID _hostID );
		
		void OnReplicationSuccess( const HostID _hostID, const PacketTag _packetTag );
		void OnReplicationFail( const HostID _hostID, const PacketTag _packetTag );

		EcsWorld* world = nullptr;	
		std::vector<HostData> hostDatas;
	};
}
