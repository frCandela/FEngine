#include "network/singletonComponents/fanServerConnectionManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanServerReplicationManager.hpp"
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
		connection.clients.clear();
		connection.onClientCreated.Clear();
		connection.onClientDeleted.Clear();
		connection.serverPort = 53000;
		connection.pingDelay = .5f;
		connection.timeoutTime = 10.f;
	}

	//================================================================================================================================
	// returns the client data associated with an ip/port, returns nullptr if it doesn't exists
	//================================================================================================================================
	HostID ServerConnectionManager::FindClient( const sf::IpAddress _ip, const unsigned short _port )
	{
		for( Client& client : clients )
		{
			if( client.state != Client::State::Null && client.ip == _ip && client.port == _port )
			{
				return client.hostId;
			}
		}
		return -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	HostID ServerConnectionManager::CreateClient( const sf::IpAddress _ip, const unsigned short _port )
	{
		assert( FindClient( _ip, _port ) == -1 );

		// find an empty slot for the new client
		Client* newClient = nullptr;
		for (int i = 0; i < clients.size() ; i++)
		{
			Client& client = clients[i];
			if( client.state == Client::State::Null )
			{
				newClient = &client;
				assert( client.hostId == i );
			}
		}

		// creates a new slot
		if( newClient == nullptr )
		{
			newClient = &clients.emplace_back();
			newClient->hostId = HostID( clients.size() - 1 );
		}

		newClient->ip = _ip;
		newClient->port = _port;
		newClient->name = "Unknown";
		newClient->state = Client::State::Disconnected;
		newClient->rtt = 0.f;
		newClient->lastResponseTime = 0.f;
		newClient->lastPingTime = 0.f;

		onClientCreated.Emmit( newClient->hostId );

		return newClient->hostId;
	}

	//================================================================================================================================
	// sends a login packet to the clients needing approval
	// regularly sends ping to clients to calculate RTT & sync frame index
	//================================================================================================================================
	void ServerConnectionManager::Send( Packet& _packet, const HostID _clientID, const FrameIndex	_frameIndex )
	{
		// Send login packet
		Client& client = clients[_clientID];
		if( client.state == Client::State::NeedingApprouval )
		{
			PacketLoginSuccess packetLogin;
			packetLogin.Write( _packet );
			_packet.onSuccess.Connect( &ServerConnectionManager::OnLoginSuccess, this );
			_packet.onFail.Connect( &ServerConnectionManager::OnLoginFail, this );
			client.state = Client::State::PendingApprouval;
		}
		else if( client.state == Client::State::Connected )
		{
			// Ping client
			const double currentTime = Time::Get().ElapsedSinceStartup();
			if( currentTime - client.lastPingTime > pingDelay )
			{
				client.lastPingTime = currentTime;

				PacketPing packetPing;
				packetPing.previousRtt = client.rtt;
				packetPing.serverFrame = _frameIndex;
				packetPing.Write( _packet );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::DetectClientTimout()
	{
		for ( Client& client : clients )
		{
			if( client.state == Client::State::Connected )
			{
				const double currentTime = Time::Get().ElapsedSinceStartup();
				if( client.lastResponseTime + timeoutTime < currentTime )
				{
					Debug::Log() << "client " << client.hostId << " timeout " << Debug::Endl();	
					DeleteClient( client.hostId );
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::DeleteClient( const HostID _clientID )
	{
		Debug::Log() << "client " << _clientID << " disconnected " << Debug::Endl();
		onClientDeleted.Emmit( _clientID );
		Client& client = clients[_clientID];
		client.state = Client::State::Null;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnLoginFail( const HostID _clientID, const PacketTag /*_packetTag*/ )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::State::PendingApprouval )
		{
			Debug::Log() << "client " << _clientID << " login failed, resending approval. " << Debug::Endl();
			client.state = Client::State::NeedingApprouval;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnLoginSuccess( const HostID _clientID, const PacketTag /*_packetTag*/ )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::State::PendingApprouval )
		{
			Debug::Log() << "Client " << _clientID << " connected " << Debug::Endl();
			client.state = Client::State::Connected;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::ProcessPacket( const HostID _clientID, const PacketHello& _packetHello )
	{
		Client& client = clients[_clientID];
		if( client.state == Client::State::Disconnected )
		{
			client.name = _packetHello.name;
			client.state = Client::State::NeedingApprouval;
		}
		else if( client.state == Client::State::Connected )
		{
			client.state = Client::State::NeedingApprouval;
			Debug::Log() << "Client " << _clientID << " disconnected" << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::ProcessPacket( const HostID _clientID, const PacketPing& _packetPing, const FrameIndex _frameIndex, const float _logicDelta )
	{
		const FrameIndex delta = _frameIndex - _packetPing.serverFrame; // number of frames elapsed between sending & receiving
		const FrameIndex clientCurrentFrameIndex = _packetPing.clientFrame + delta / 2;

		Client& client = clients[_clientID];
		client.rtt = _logicDelta * delta;
		client.framesDelta[client.nextDeltaIndex] = _frameIndex - clientCurrentFrameIndex;
		client.nextDeltaIndex = ( client.nextDeltaIndex + 1 ) % int(client.framesDelta.size());
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const Client::State _clientState )
	{
		switch( _clientState )
		{
		case Client::State::Null:				return "Null";				break;
		case Client::State::Disconnected:		return "Disconnected";		break;
		case Client::State::NeedingApprouval:	return "NeedingApprouval";	break;
		case Client::State::PendingApprouval:	return "PendingApprouval";	break;
		case Client::State::Connected:			return "Connected";			break;
		default:			assert( false );		return "error";				break;
		}
	}


	//================================================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//================================================================================================================================
	static ImVec4 GetStateColor( const Client::State _clientState )
	{
		switch( _clientState )
		{
		case Client::State::Null:				return  Color::Red.ToImGui(); break;
		case Client::State::Disconnected:		return  Color::Orange.ToImGui(); break;
		case Client::State::NeedingApprouval:	return  Color::Yellow.ToImGui(); break;
		case Client::State::PendingApprouval:	return  Color::Yellow.ToImGui(); break;
		case Client::State::Connected:			return  Color::Green.ToImGui(); break;
		default:			assert( false );		return  Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnectionManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			double currentTime = Time::Get().ElapsedSinceStartup();

			ServerConnectionManager& connection = static_cast<ServerConnectionManager&>( _component );

			ImGui::Text( "Server" );
			ImGui::Spacing();
			ImGui::Text( "port: %u", connection.serverPort );
			ImGui::DragFloat( "ping delay", &connection.pingDelay, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "timeout time", &connection.timeoutTime, 0.1f, 0.f, 10.f );
			// 		if( ImGui::Button( "Start" ) && _gameServer.state == GameServer::WAITING_FOR_PLAYERS )
			// 		{
			// 			_gameServer.state = GameServer::STARTING;
			// 			game.frameStart = game.frameIndex + 180;
			// 		}

			// draw all clients
			if( ImGui::CollapsingHeader( "clients" ) )
			{
				ImGui::Columns( 7 );
				ImGui::Text( "name" );			ImGui::NextColumn();
				ImGui::Text( "state" );			ImGui::NextColumn();
				ImGui::Text( "rtt" );			ImGui::NextColumn();
				ImGui::Text( "bandwidth" );			ImGui::NextColumn();
				ImGui::Text( "last response" );	ImGui::NextColumn();
				ImGui::Text( "adress" );		ImGui::NextColumn();				
				ImGui::Text( "frame delta" );	ImGui::NextColumn();

				for( int i = (int)connection.clients.size() - 1; i >= 0; i-- )
				{					
					Client& client = connection.clients[i];
					ImGui::Text( client.name.c_str());
					ImGui::NextColumn();

					ImGui::TextColored( GetStateColor( client.state ), GetStateName( client.state ).c_str() );
					ImGui::NextColumn();

					ImGui::TextColored( GetRttColor( client.rtt ), "%.1f", 1000.f * client.rtt );
					ImGui::NextColumn();

					ImGui::Text( "%.1f Ko/s", client.bandwidth );
					ImGui::NextColumn();

					ImGui::Text( "%.1f", currentTime - client.lastResponseTime );
					ImGui::NextColumn();

					ImGui::Text( "%s::%u", client.ip.toString().c_str(), client.port );
					ImGui::NextColumn();
 					
					ImGui::Text( "%d %d %d %d %d", client.framesDelta[0], client.framesDelta[1], client.framesDelta[2], client.framesDelta[3], client.framesDelta[4] );
					ImGui::NextColumn();
				}
				ImGui::Columns( 1 );
			}
		}ImGui::Unindent(); ImGui::Unindent();
	}
}