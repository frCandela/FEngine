#include "network/components/fanClientRPC.hpp"
#include "network/singletons/fanSpawnManager.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientRPC::SetInfo( EcsComponentInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientRPC::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        ClientRPC& rpc = static_cast<ClientRPC&>( _component );
        rpc.mNameToRPCTable.clear();
        rpc.mOnShiftFrameIndex.Clear();
        rpc.mOnSpawn.Clear();
        rpc.RegisterRPCs();
    }

    //==================================================================================================================================================================================================
    // register one rpc unwrap method, checks that there is no RpcID collision
    //==================================================================================================================================================================================================
    void ClientRPC::RegisterUnwrapMethod( const RpcID _rpcID, const RpcUnwrapMethod _unwrapMethod )
    {
        fanAssert( mNameToRPCTable.find( _rpcID ) == mNameToRPCTable.end() );
        mNameToRPCTable[_rpcID] = _unwrapMethod;
    }

    //==================================================================================================================================================================================================
    // Registers  RPC unwrap functions
    // A unwrap function must decode a packet and run the corresponding procedure
    //==================================================================================================================================================================================================
    void ClientRPC::RegisterRPCs()
    {
        mNameToRPCTable.clear();
        RegisterUnwrapMethod( sRpcIdShiftFrame, &ClientRPC::UnwrapShiftClientFrame );
        RegisterUnwrapMethod( sRpcIdSpawn, &ClientRPC::UnwrapSpawn );
        RegisterUnwrapMethod( sRpcIdDespawn, &ClientRPC::UnwrapDespawn );
    }

    //==================================================================================================================================================================================================
    // Runs a procedure from an incoming rpc packet data
    //==================================================================================================================================================================================================
    void ClientRPC::TriggerRPC( sf::Packet& _packet )
    {
        RpcID rpcID;
        _packet >> rpcID;

        const RpcUnwrapMethod method = mNameToRPCTable[rpcID];
        ( ( *this ).*( method ) )( _packet );
    }

    //==================================================================================================================================================================================================
    // SynchClientFrame RPC
    //==================================================================================================================================================================================================
    PacketReplication ClientRPC::RPCShiftClientFrame( const int _framesDelta )
    {
        PacketReplication packet;
        packet.mReplicationType = PacketReplication::ReplicationType::RPC;

        packet.mPacketData.clear();
        packet.mPacketData << sRpcIdShiftFrame;
        packet.mPacketData << sf::Int32( _framesDelta );

        return packet;
    }

    //==================================================================================================================================================================================================
    // Must be connected to the SpawnManager, copy packet data for future spawning
    //==================================================================================================================================================================================================
    void ClientRPC::UnwrapSpawn( sf::Packet& _packet )
    {
        FrameIndex frameIndex;
        SpawnID    spawnID;
        _packet >> spawnID;
        _packet >> frameIndex;
        mOnSpawn.Emmit( spawnID, frameIndex, _packet );
    }

    //==================================================================================================================================================================================================
    // Generate a RPC replication packet for spawning entities
    //==================================================================================================================================================================================================
    PacketReplication ClientRPC::RPCSpawn( const SpawnInfo& _spawnInfo )
    {
        PacketReplication packet;
        packet.mReplicationType = PacketReplication::ReplicationType::RPC;

        packet.mPacketData.clear();
        packet.mPacketData << sRpcIdSpawn;
        packet.mPacketData << _spawnInfo.spawnID;
        packet.mPacketData << _spawnInfo.spawnFrameIndex;

        sf::Packet dataCpy = _spawnInfo.data;
        while( !dataCpy.endOfPacket() )
        {
            uint8_t dataByte;
            dataCpy >> dataByte;
            packet.mPacketData << dataByte;
        }

        return packet;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientRPC::UnwrapDespawn( sf::Packet& _packet )
    {
        NetID netId;
        _packet >> netId;
        mOnDespawn.Emmit( netId );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    PacketReplication ClientRPC::RPCDespawn( const NetID _netID )
    {
        PacketReplication packet;
        packet.mReplicationType = PacketReplication::ReplicationType::RPC;

        packet.mPacketData.clear();
        packet.mPacketData << sRpcIdDespawn;
        packet.mPacketData << _netID;

        return packet;
    }

    //==================================================================================================================================================================================================
    // ShiftClientFrame RPC - unwrap data & synchronizes the frame index of the client depending on its rtt
    //==================================================================================================================================================================================================
    void ClientRPC::UnwrapShiftClientFrame( sf::Packet& _packet )
    {
        sf::Int32 framesDelta;
        _packet >> framesDelta;
        mOnShiftFrameIndex.Emmit( framesDelta );
    }
}