#include "network/components/fanClientReplication.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "network/components/fanClientRPC.hpp"

namespace fan
{
	REGISTER_COMPONENT( ClientReplication, "client replication" );

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientReplication::Init;
		_info.onGui = &ClientReplication::OnGui;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::Init( EcsWorld& _world, Component& _component )
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
		case PacketReplication::ReplicationType::Component:				assert( false );								break;
		case PacketReplication::ReplicationType::SingletonComponent:	replicationListSingletons.push_back( _packet );	break;
		case PacketReplication::ReplicationType::RPC:					replicationListRPC.push_back( _packet );		break;
		default:														assert( false );								break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplication::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "singletons: %d", replicationManager.replicationListSingletons.size() );
			ImGui::Text( "rpc:        %d", replicationManager.replicationListRPC.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}