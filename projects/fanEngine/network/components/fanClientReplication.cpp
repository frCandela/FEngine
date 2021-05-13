#include "network/components/fanClientReplication.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientReplication::SetInfo( EcsComponentInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
        replicationManager.mReplicationListRPC.clear();
        replicationManager.mReplicationListSingletons.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientReplication::ProcessPacket( PacketReplication& _packet )
    {
        switch( _packet.mReplicationType )
        {
            case PacketReplication::ReplicationType::Entity:
                mReplicationListEntities.push_back( _packet );
                break;
            case PacketReplication::ReplicationType::SingletonComponent:
                mReplicationListSingletons.push_back( _packet );
                break;
            case PacketReplication::ReplicationType::RPC:
                mReplicationListRPC.push_back( _packet );
                break;
            default:
                fanAssert( false );
                break;
        }
    }
}