#pragma once

#include <unordered_map>
#include "core/ecs/fanEcsComponent.hpp"
#include "core/ecs/fanSignal.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    struct SpawnInfo;

    //==================================================================================================================================================================================================
    // Manages remote procedure calls
    // A RPC is the act of one host causing a	procedure execution one one or more remote hosts
    // Can generate replication packets to send using the server replication
    //==================================================================================================================================================================================================
    struct ClientRPC : public EcsComponent
    {
    ECS_COMPONENT( ClientRPC )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

        using RpcID = sf::Uint32;
        using RpcUnwrapMethod = void ( ClientRPC::* )( sf::Packet& );

        void RegisterUnwrapMethod( const RpcID _rpcID, const RpcUnwrapMethod _unwrapMethod );
        void RegisterRPCs();
        void TriggerRPC( sf::Packet& _packet );

        std::unordered_map<RpcID, RpcUnwrapMethod> mNameToRPCTable;

        // List of available rpc
        Signal<int>        mOnShiftFrameIndex;
        static const RpcID sRpcIdShiftFrame                  = SSID( "RPCShiftFrame" );
        void UnwrapShiftClientFrame( sf::Packet& _packet );
        static PacketReplication RPCShiftClientFrame( const int _framesDelta );

        Signal <SpawnID, FrameIndex, sf::Packet> mOnSpawn;
        static const RpcID                       sRpcIdSpawn = SSID( "RPCSpawn" );
        void UnwrapSpawn( sf::Packet& _packet );
        static PacketReplication RPCSpawn( const SpawnInfo& _spawnInfo );

        Signal <NetID>     mOnDespawn;
        static const RpcID sRpcIdDespawn                     = SSID( "RPCDespawn" );
        void UnwrapDespawn( sf::Packet& _packet );
        static PacketReplication RPCDespawn( const NetID _netID );
    };
}