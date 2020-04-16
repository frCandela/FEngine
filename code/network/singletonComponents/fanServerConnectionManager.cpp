#include "network/singletonComponents/fanServerConnectionManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"


namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerConnectionManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &ServerConnectionManager::Init;
		_info.onGui = &ServerConnectionManager::OnGui;
		_info.name = "server connection manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerConnectionManager& connection = static_cast<ServerConnectionManager&>( _component );
		connection.clients.clear();
		connection.serverPort = 53000;
		connection.pingDuration = 0.1f;		// clients are pinged every X seconds
		connection.timeoutDuration = 3.f;	// clients are disconnected after X seconds
	}

	//================================================================================================================================
	// returns the client data associated with an ip/port, returns nullptr if it doesn't exists
	//================================================================================================================================
	HostID ServerConnectionManager::FindClient( const sf::IpAddress _ip, const unsigned short _port )
	{
		for( Client& client : clients )
		{
			if( client.ip == _ip && client.port == _port )
			{
				return client.clientId;
			}
		}
		return -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	HostID ServerConnectionManager::CreateClient( const sf::IpAddress _ip, const unsigned short _port )
	{
		assert( FindClient( _ip, _port ) == -1 );

		Client newClient;
		newClient.clientId = (HostID)clients.size();
		newClient.ip = _ip;
		newClient.port = _port;
		newClient.state = Client::ClientState::Disconnected;
		newClient.name = "Unknown";
		clients.push_back( newClient );
		return newClient.clientId;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::Send( Packet& _packet, const HostID _clientID )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::ClientState::NeedingApprouval )
		{
			PacketLoginSuccess packetLogin;
			packetLogin.Save( _packet );
			_packet.onSuccess.Connect( &ServerConnectionManager::OnLoginSuccess, this );
			_packet.onFail.Connect( &ServerConnectionManager::OnLoginFail, this );
			client.state = Client::ClientState::PendingApprouval;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnLoginFail( const HostID _clientID )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::ClientState::PendingApprouval )
		{
			Debug::Log() << "client " << _clientID << " login failed, resending approval. " << Debug::Endl();
			client.state = Client::ClientState::NeedingApprouval;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnLoginSuccess( const HostID _clientID )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::ClientState::PendingApprouval )
		{
			Debug::Log() << "Client " << _clientID << " connected ! " << Debug::Endl();
			client.state = Client::ClientState::Connected;
		}
	}

	//================================================================================================================================
	// 
	//================================================================================================================================
	void ServerConnectionManager::ProcessPacket( const HostID _clientID, const PacketHello& _packetHello )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::ClientState::Disconnected )
		{
			client.name = _packetHello.name;
			client.state = Client::ClientState::NeedingApprouval;
		}
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string ToString( Client::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::Client::ClientState::Disconnected:		return "Disconnected";		break;
		case fan::Client::ClientState::NeedingApprouval:	return "NeedingApprouval";	break;
		case fan::Client::ClientState::PendingApprouval:	return "PendingApprouval";	break;
		case fan::Client::ClientState::Connected:			return "Connected";			break;
		default:			assert( false );				return "error";				break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnGui( SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			double currentTime = Time::Get().ElapsedSinceStartup();

			ServerConnectionManager& connection = static_cast<ServerConnectionManager&>( _component );

			ImGui::Text( "Server" );
			ImGui::Spacing();
			ImGui::Text( "port: %u", connection.serverPort );
			ImGui::DragFloat( "ping duration", &connection.pingDuration, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "timeout duration", &connection.timeoutDuration, 0.1f, 0.f, 10.f );
			// 		if( ImGui::Button( "Start" ) && _gameServer.state == GameServer::WAITING_FOR_PLAYERS )
			// 		{
			// 			_gameServer.state = GameServer::STARTING;
			// 			game.frameStart = game.frameIndex + 180;
			// 		}

					// draw all clients
			if( ImGui::CollapsingHeader( "clients" ) )
			{
				for( int i = (int)connection.clients.size() - 1; i >= 0; i-- )
				{
					Client& client = connection.clients[i];

					ImGui::Text( "name           %s", client.name.c_str() );
					ImGui::Text( "state:         %s", ToString( client.state ).c_str() );
					ImGui::Text( "adress         %s::%u", client.ip.toString().c_str(), client.port );
					ImGui::Text( "ping           %.01f", .5f * 1000.f * client.roundTripDelay );
					ImGui::Text( "last response  %.1f", currentTime - client.lastResponse );
					ImGui::Spacing();
				}
			}
		}ImGui::Unindent(); ImGui::Unindent();
	}
}