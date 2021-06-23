#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // [Server] Sends packets to clients to replicates objects / run RPC
    //==================================================================================================================================================================================================
    struct HostReplication : public EcsComponent
    {
    ECS_COMPONENT( HostReplication )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

        enum ReplicationFlags
        {
            None                  = 0,
            ResendUntilReplicated = 1 << 1    // if the packets drops, resend it
        };

        //================================================================
        // replication data for a singleton component
        //================================================================
        struct ReplicationData
        {
            ReplicationFlags  mFlags = ReplicationFlags::None;    // replication parameters
            PacketReplication mPacket;                            // saved replication data
            Signal<>          mOnSuccess;
        };

        std::multimap<PacketTag, ReplicationData> mPendingReplication; // sent, waiting for a status
        std::vector<ReplicationData>              mNextReplication;    // waiting to be sent

        void Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet );

        Signal<>& Replicate( const PacketReplication& _packet, const ReplicationFlags _flags );
        void OnReplicationSuccess( const PacketTag _packetTag );
        void OnReplicationFail( const PacketTag _packetTag );

        static PacketReplication BuildSingletonPacket( const EcsWorld& _world, const uint32_t _staticID );
        static PacketReplication BuildEntityPacket( EcsWorld& _world,
                                                    const EcsHandle _handle,
                                                    const std::vector<uint32_t>& _componentTypeInfo );
        static PacketReplication BuildRPCPacket( sf::Packet& _dataRPC );
    };
}