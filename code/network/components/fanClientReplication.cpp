#include "network/components/fanClientReplication.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletons/fanGame.hpp"
#include "network/components/fanClientRPC.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &ClientReplication::OnGui;
		_info.name = "client replication";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
		replicationManager.replicationListRPC.clear();
		replicationManager.replicationListSingletons.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::ProcessPacket( PacketReplication& _packet )
	{
		switch( _packet.replicationType )
		{
		case PacketReplication::ReplicationType::Entity:				replicationListEntities.push_back( _packet );	break;
		case PacketReplication::ReplicationType::SingletonComponent:	replicationListSingletons.push_back( _packet );	break;
		case PacketReplication::ReplicationType::RPC:					replicationListRPC.push_back( _packet );		break;
		default:														assert( false );								break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );

		ImGui::Text( "singletons: %d", replicationManager.replicationListSingletons.size() );
		ImGui::Text( "rpc:        %d", replicationManager.replicationListRPC.size() );
	}
}