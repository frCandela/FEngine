#include "network/singletonComponents/fanServerNetworkManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanServerReplicationManager.hpp"
#include "network/singletonComponents/fanServerConnectionManager.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerNetworkManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ServerNetworkManager::Init;
		_info.onGui = &ServerNetworkManager::OnGui;
		_info.save = &ServerNetworkManager::Save;
		_info.load = &ServerNetworkManager::Load;
		_info.name = "server network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
		netManager.hostDatas.clear();
		netManager.syncInterval = 2.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::CreateHost( const HostID _hostID )
	{
		if( _hostID >= hostDatas.size() )
		{
			hostDatas.resize( _hostID + 1 );
		}

		HostData& hostData = hostDatas[_hostID];
		hostData = {};
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::DeleteHost( const HostID _hostID )
	{
		hostDatas[_hostID].isNull = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Update( EcsWorld& _world, const HostID _hostID )
	{
		const double currentTime = Time::Get().ElapsedSinceStartup();
		HostData& hostData = hostDatas[_hostID];
		if( !hostData.isNull )
		{
			if( currentTime > hostData.lastSync + syncInterval )
			{
				hostData.lastSync = currentTime;

				RPCManager& rpcManager = _world.GetSingletonComponent<RPCManager>();
				ServerReplicationManager& replication = _world.GetSingletonComponent<ServerReplicationManager>();
				ServerConnectionManager& connection = _world.GetSingletonComponent<ServerConnectionManager>();
				Game& game = _world.GetSingletonComponent<Game>();

				replication.ReplicateOnAllClients(
					rpcManager.RPCSyncClientFrame( game.frameIndex, connection.clients[_hostID].roundTripTime )
					, ServerReplicationManager::None
				);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ServerNetworkManager& netManager = static_cast<const ServerNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			for (int i = 0; i < netManager.hostDatas.size(); i++)
			{
				HostData& data = netManager.hostDatas[i];
				if( !data.isNull )
				{
					ImGui::Text( "client %d", i );
					ImGui::Spacing(); ImGui::Spacing();
				}
			}
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}