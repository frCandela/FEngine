#include "network/singletonComponents/fanClientReplicationManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientReplicationManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientReplicationManager::Init;
		_info.onGui = &ClientReplicationManager::OnGui;
		_info.save = &ClientReplicationManager::Save;
		_info.load = &ClientReplicationManager::Load;
		_info.name = "replication manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientReplicationManager& replicationManager = static_cast<ClientReplicationManager&>( _component );
		replicationManager.replicationListRPC.clear();
		replicationManager.replicationListSingletons.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::ProcessPacket( PacketReplication& _packet )
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
	void ClientReplicationManager::ReplicateRPC( RPCManager& _rpcManager )
	{
		for( PacketReplication packet : replicationListRPC )
		{
			_rpcManager.TriggerRPC( packet.packetData );
		}
		replicationListRPC.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::ReplicateSingletons( EcsWorld& _world )
	{
		for ( PacketReplication packet : replicationListSingletons )
		{
			sf::Uint32 staticIndex;
			packet.packetData >> staticIndex;
			SingletonComponent& singleton = _world.GetSingletonComponent( staticIndex );
			const SingletonComponentInfo& info = _world.GetSingletonComponentInfo( staticIndex );
			info.netLoad( singleton, packet.packetData );
			assert( packet.packetData.endOfPacket() );
		}
		replicationListSingletons.clear();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ClientReplicationManager& replicationManager = static_cast<const ClientReplicationManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ClientReplicationManager& replicationManager = static_cast<ClientReplicationManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ClientReplicationManager& replicationManager = static_cast<ClientReplicationManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "singletons: %d", replicationManager.replicationListSingletons.size() );
			ImGui::Text( "rpc:        %d", replicationManager.replicationListRPC.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}