#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientRPC.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &ClientReplication::OnGui;
		_info.mName  = "client replication";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
		replicationManager.mReplicationListRPC.clear();
		replicationManager.mReplicationListSingletons.clear();
	}

	//========================================================================================================
	//========================================================================================================
	void ClientReplication::ProcessPacket( PacketReplication& _packet )
	{
		switch( _packet.replicationType )
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
		    assert( false );
		    break;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientReplication::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
		ImGui::Text( "singletons: %d", replicationManager.mReplicationListSingletons.size() );
		ImGui::Text( "rpc:        %d", replicationManager.mReplicationListRPC.size() );
	}
}