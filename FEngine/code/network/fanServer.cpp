#include "fanGlobalIncludes.h"
#include "network/fanServer.h"

#include "network/packets/fanPacketLogin.h"
#include "network/packets/fanPacketPing.h"
#include "core/time/fanTime.h"
#include "core/input/fanInput.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Server::Server( ) 		
	{
		m_socket.setBlocking( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::Create( const Port _listenPort )
	{
		m_listenPort = _listenPort;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Server::Bind()
	{
		if ( m_socket.bind( m_listenPort ) != sf::Socket::Done )
		{
			Debug::Warning() << "[SERVER] bind failed on port " << m_listenPort << Debug::Endl();
			return false;
		}

		m_state = State::WAITING_PLAYERS;
		Debug::Log() << "[SERVER] bind success on port " << m_listenPort << Debug::Endl();
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::UnBind()
	{
		m_socket.unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::Update( const float _delta )
	{
		if ( m_state != State::NONE )
		{
			while( Receive() == true ){}

			for (int clientIndex = 0; clientIndex < m_clients.size() ; clientIndex++)
			{
				ClientData& client = m_clients[clientIndex];
				UpdateClient( client, _delta );
			}
		}		

		switch ( m_state )
		{
		case State::NONE:
			break;
		case State::WAITING_PLAYERS:

			break;
		default:
			break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::UpdateClient( ClientData& _client, const float _delta )
	{
		_client.lastResponse += _delta;

		if ( _client.lastResponse > 2.f )
		{
			RemoveClient(_client);
		}
		else if ( _client.lastResponse > 1.f )
		{
			PacketPing ping( Time::Get().ElapsedSinceStartup() );
			SendPacket( _client, ping.ToPacket() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::ProcessPacket( const sf::IpAddress _ip, const Port _port, sf::Packet& _packet )
	{
		ClientData * client = FindClient( _ip , _port );

		// Process packet
		sf::Uint16 intType;
		_packet >> intType;
		const PacketType type = PacketType( intType );

		if ( client == nullptr )
		{
			if ( type == PacketType::LOGIN )
			{
				PacketLogin packetLogin;
				packetLogin.LoadFrom( _packet );
				client = AddClient( _ip, _port, packetLogin );
				SendPacket( *client, PacketAckLogin().ToPacket() );
			}
			else
			{
				Debug::Log() << "[SERVER] drunk client " << Debug::Endl();
			}
		}
		else
		{
			client->lastResponse = 0.f;

			switch ( type )
			{
			case PacketType::PING: {
				PacketPing packetPing;
				packetPing.LoadFrom( _packet );
				const float time = Time::Get().ElapsedSinceStartup();
				client->ping = time - packetPing.GetTime();
			} break;
			default:
				Debug::Log() << "[SERVER] strange packet received with id:  " << intType << Debug::Endl();
				break;
			}
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	void Server::BroadcastPacket( sf::Packet _packet )
	{
		for (int clientIndex = 0; clientIndex < m_clients.size() ; clientIndex++)
		{
			ClientData& client = m_clients[clientIndex];
			SendPacket( client, _packet );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::SendPacket( const ClientData& _client, sf::Packet _packet )
	{
		if ( m_socket.send( _packet, _client.ipAdress, _client.port ) != sf::Socket::Done )
		{
			Debug::Warning() << "[SERVER] packet send failed for client " << _client.name << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Server::Receive()
	{
		// Receive packet
		sf::Packet		packet;
		sf::IpAddress	clientIp;
		Port			clientPort;
		sf::UdpSocket::Status	status = m_socket.receive( packet, clientIp, clientPort );

		if ( status == sf::UdpSocket::Done )
		{
			if ( clientPort != m_listenPort )
			{
				ProcessPacket( clientIp, clientPort, packet );
				return true;
			}
			
		}
		else if ( status ==  sf::UdpSocket::Disconnected )
		{
			ClientData * client = FindClient( clientIp , clientPort );
			if ( client != nullptr )
			{
				RemoveClient(*client);
			}
			
		}
		else if ( status == sf::UdpSocket::NotReady  )
		{
			// do nothing
		}
		else
		{
			Debug::Warning() << "[SERVER] receive error" << Debug::Endl();
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	Server::ClientData * Server::AddClient( const sf::IpAddress _ip, const Port _port, const PacketLogin& _loginInfo )
	{
		assert( FindClient(_ip,_port) == nullptr  );

		ClientData client;
		client.ipAdress = _ip;
		client.port = _port;
		client.name = _loginInfo.GetName();
		m_clients.push_back( client );
		onClientConnected.Emmit( (int)m_clients.size() - 1, client.name );

		Debug::Log() << "[SERVER] client connected " << client.name << " " << client.ipAdress.toString() << "::" << client.port << Debug::Endl();

		return &m_clients[m_clients.size() - 1];
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::RemoveClient( ClientData& _client )
	{
		for (int clientIndex = (int)m_clients.size() - 1; clientIndex >= 0  ; --clientIndex )
		{
			if ( &m_clients[clientIndex] == &_client )
			{
				Debug::Log() << "[SERVER] client disconnected: " << _client.name << " " << _client.ipAdress.toString() << "::" << _client.port << Debug::Endl();
				m_clients.erase( m_clients.begin() + clientIndex );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Server::ClearClients()
	{
		while ( m_clients.size() > 0 )
		{
			RemoveClient( m_clients[m_clients.size() - 1] );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Server::ClientData * Server::FindClient( const sf::IpAddress _ip, const Port _port )
	{
		for ( int clientIndex = 0; clientIndex < m_clients.size(); ++clientIndex )
		{
			ClientData & client = m_clients[clientIndex];
			if ( client.ipAdress == _ip && client.port == _port )
			{
				return &client;
			}
		}
		return nullptr;
	}
}