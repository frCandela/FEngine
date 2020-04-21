#include "network/singletonComponents/fanClientReplicationManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientReplicationManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NONE;
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
		replicationManager.world = &_world;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientReplicationManager::ProcessPacket( PacketReplicationSingletonComponents& _packet )
	{
		_packet.ReplicateOnWorld( *world );
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
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}