#include "game/network/fanOldGameServer.hpp"

#include "network/packets/fanPacketLogin.hpp"
#include "network/packets/fanPacketPing.hpp"
#include "network/packets/fanIPacket.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanJoystick.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{

		//================================================================================================================================
		//================================================================================================================================
		void OldGameServer::OnAttach()
	{
// 		//Actor::OnAttach();
// 		m_socket.Create( "[SERVER]", 53000 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::OnDetach()
	{
// 		//Actor::OnDetach();
// 		m_socket.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::Start()
	{
// 		// Remove this if we are on a client scene HACK?
// 		if ( !GetScene().IsServer() )
// 		{
// 			GetScene().DeleteComponent( this );
// 			return;
// 		}
// 
// 		REQUIRE_COMPONENT( PlayersManager, m_playersManager );
// 		REQUIRE_COMPONENT( GameManager, m_gameManager );
// 
// 		if ( m_playersManager )
// 		{
// 			onClientConnected.Connect( &PlayersManager::AddPlayer, m_playersManager );
// 		}
// 
// 		m_socket.Bind();
// 		m_state = ServerState::SERVER_WAITING_PLAYERS;
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::Stop()
	{
// 		ClearClients();
// 		onClientConnected.Disconnect( &PlayersManager::AddPlayer, m_playersManager );
// 
// 		m_socket.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::Update( const float _delta )
	{
// 		if ( m_state != ServerState::SERVER_NONE )
// 		{
// 			Receive();
// 
// 			for ( int clientIndex = 0; clientIndex < m_clients.size(); clientIndex++ )
// 			{
// 				ClientData& client = m_clients[ clientIndex ];
// 				UpdateClient( client, _delta );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::Receive()
	{
// 		sf::Packet		packet;
// 		bool disconnected = false;
// 		while ( m_socket.Receive( packet, disconnected ) )
// 		{
// 			if ( disconnected )
// 			{
// 				ClientData* client = FindClient( m_socket.GetLastReceiveIp(), m_socket.GetLastReceivePort() );
// 				if ( client != nullptr )
// 				{
// 					RemoveClient( *client );
// 				}
// 			}
// 			else if ( m_socket.GetLastReceivePort() != m_socket.GetPort() )
// 			{
// 				ProcessPacket( m_socket.GetLastReceiveIp(), m_socket.GetLastReceivePort(), packet );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::ProcessPacket( const sf::IpAddress& _ip, const Port& _port, sf::Packet& _packet )
	{
// 		ClientData* client = FindClient( _ip, _port );
// 
// 		// Process packet
// 		sf::Uint16 intType;
// 		_packet >> intType;
// 		const PacketType type = PacketType( intType );
// 
// 		if ( client == nullptr )
// 		{
// 			if ( type == PacketType::LOGIN )
// 			{
// 				PacketLogin packetLogin;
// 				packetLogin.LoadFrom( _packet );
// 				client = AddClient( _ip, _port, packetLogin );
// 				Send( PacketAckLogin().ToPacket(), *client );
// 			}
// 			else
// 			{
// 				Debug::Log() << "[SERVER] drunk client " << Debug::Endl();
// 			}
// 		}
// 		else
// 		{
// 			client->lastResponse = 0.f;
// 
// 			switch ( type )
// 			{
// 			case PacketType::PING:
// 			{
// 				PacketPing packetPing;
// 				packetPing.LoadFrom( _packet );
// 				const float time = Time::Get().ElapsedSinceStartup();
// 				client->ping = time - packetPing.GetTime();
// 			} break;
// 			default:
// 				Debug::Log() << "[SERVER] strange packet received with id:  " << intType << Debug::Endl();
// 				break;
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool OldGameServer::Send( sf::Packet _packet, const ClientData& _client )
	{
/*		return m_socket.Send( _packet, _client.ipAdress, _client.port );*/
		return false;
	}

	//================================================================================================================================
	// Send to all clients
	//================================================================================================================================
	void OldGameServer::Broadcast( sf::Packet _packet )
	{
// 		for ( int clientIndex = 0; clientIndex < m_clients.size(); clientIndex++ )
// 		{
// 			ClientData& client = m_clients[ clientIndex ];
// 			Send( _packet, client );
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::UpdateClient( ClientData& _client, const float _delta )
	{
// 		_client.lastResponse += _delta;
// 
// 		if ( _client.lastResponse > 2.f )
// 		{
// 			RemoveClient( _client );
// 		}
// 		else if ( _client.lastResponse > 1.f )
// 		{
// 			PacketPing ping( Time::Get().ElapsedSinceStartup() );
// 			Send( ping.ToPacket(), _client );
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::StartGame()
	{
// 		Broadcast( PacketStartGame().ToPacket() );
// 		m_playersManager->SpawnSpaceShips();
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::LateUpdate( const float /*_delta*/ ) {}

	//================================================================================================================================
	//================================================================================================================================
	OldGameServer::ClientData* OldGameServer::AddClient( const sf::IpAddress& _ip, const Port& _port, const PacketLogin& _loginInfo )
	{
		assert( FindClient( _ip, _port ) == nullptr );

		ClientData client;
		client.ipAdress = _ip;
		client.port = _port;
		client.name = _loginInfo.GetName();
		m_clients.push_back( client );
		onClientConnected.Emmit( ( int ) m_clients.size() - 1, client.name );

		Debug::Log() << "[SERVER] client connected " << client.name << " " << client.ipAdress.toString() << "::" << client.port << Debug::Endl();

		return &m_clients[ m_clients.size() - 1 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::RemoveClient( ClientData& _client )
	{
		for ( int clientIndex = ( int ) m_clients.size() - 1; clientIndex >= 0; --clientIndex )
		{
			if ( &m_clients[ clientIndex ] == &_client )
			{
				Debug::Log() << "[SERVER] client disconnected: " << _client.name << " " << _client.ipAdress.toString() << "::" << _client.port << Debug::Endl();
				m_clients.erase( m_clients.begin() + clientIndex );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::ClearClients()
	{
		while ( m_clients.size() > 0 )
		{
			RemoveClient( m_clients[ m_clients.size() - 1 ] );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	OldGameServer::ClientData* OldGameServer::FindClient( const sf::IpAddress& _ip, const Port& _port )
	{
		for ( int clientIndex = 0; clientIndex < m_clients.size(); ++clientIndex )
		{
			ClientData& client = m_clients[ clientIndex ];
			if ( client.ipAdress == _ip && client.port == _port )
			{
				return &client;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameServer::OnGui()
	{


		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			if ( ImGui::Button( "start game " ) )
			{
				StartGame();
			}
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool OldGameServer::Load( const Json& _json )
	{

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool OldGameServer::Save( Json& _json ) const
	{

		return true;
	}
}