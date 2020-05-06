#include "network/singletonComponents/fanServerConnectionManager.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/components/fanHostConnection.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerConnectionManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ServerConnectionManager::Init;
		_info.onGui = &ServerConnectionManager::OnGui;
		_info.name = "server connection manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerConnectionManager& connection = static_cast<ServerConnectionManager&>( _component );
		connection.serverPort = 53000;
		connection.pingDelay = .5f;
		connection.timeoutTime = 10.f;
	}

	//================================================================================================================================
	// sends a login packet to the clients needing approval
	// regularly sends ping to clients to calculate RTT & sync frame index
	//================================================================================================================================
	void ServerConnectionManager::Write( EcsWorld& _world, Packet& _packet, const HostID _clientID )
	{
		ServerConnectionManager& netManager = _world.GetSingletonComponent<ServerConnectionManager>();
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		const Game& game = _world.GetSingletonComponent<Game>();

		const EntityHandle handle = hostManager.hostHandles[_clientID];
		const EntityID entityID = _world.GetEntityID( handle );
		HostConnection& connection = _world.GetComponent<HostConnection>( entityID );

		game.frameIndex;

		// Send login packet
		if( connection.state == HostConnection::NeedingApprouval )
		{
			PacketLoginSuccess packetLogin;
			packetLogin.Write( _packet );
			_packet.onSuccess.Connect( &HostConnection::OnLoginSuccess, &connection );
			_packet.onFail.Connect( &HostConnection::OnLoginFail, &connection );
			connection.state = HostConnection::PendingApprouval;
		}
		else if( connection.state == HostConnection::Connected )
		{
			// Ping client
			const double currentTime = Time::Get().ElapsedSinceStartup();
			if( currentTime - connection.lastPingTime > pingDelay )
			{
				connection.lastPingTime = currentTime;

				PacketPing packetPing;
				packetPing.previousRtt = connection.rtt;
				packetPing.serverFrame = game.frameIndex;
				packetPing.Write( _packet );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::DetectClientTimout( EcsWorld& _world )
	{
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		for( std::pair<HostID, EntityHandle> pair : hostManager.hostHandles )
		{
			const HostID   hostID = pair.first;
			const EntityID entityID = _world.GetEntityID( pair.second );
			HostConnection connection = _world.GetComponent<HostConnection>( entityID );

			if( connection.state == HostConnection::Connected )
			{
				const double currentTime = Time::Get().ElapsedSinceStartup();
				if( connection.lastResponseTime + timeoutTime < currentTime )
				{
					Debug::Log() << "client " << connection.hostId << " timeout " << Debug::Endl();
					hostManager.DeleteHost( connection.hostId );
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerConnectionManager& connection = static_cast<ServerConnectionManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{			
			ImGui::Text( "Server" );
			ImGui::Spacing();
			ImGui::Text( "port: %u", connection.serverPort );
			ImGui::DragFloat( "ping delay", &connection.pingDelay, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "timeout time", &connection.timeoutTime, 0.1f, 0.f, 10.f );

		}ImGui::Unindent(); ImGui::Unindent();
	}
}