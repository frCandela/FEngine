#include "fanGlobalIncludes.h"
#include "network/fanServer.h"

#include "network/packets/fanPacketLogin.h"
#include "network/packets/fanPacketPing.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Server::Server( const Port _listenPort ) :
		m_listenPort( _listenPort )
	{
		m_socket.setBlocking( false );
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
	void Server::Update( const float /*_delta*/ )
	{
		if ( m_state != State::NONE )
		{
			Receive();
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
	void Server::ProcessPacket( const sf::IpAddress _ip, const Port _port, sf::Packet& _packet )
	{
		ClientData * client = FindClient( _ip , _port );

		// Process packet
		sf::Uint16 intType;
		_packet >> intType;
		const PacketType type = PacketType( intType );

		switch ( type )
		{
		case PacketType::LOGIN:
			if ( client == nullptr )
			{
				PacketLogin packetLogin;
				packetLogin.LoadFrom( _packet );
				client = AddClient( _ip, _port, packetLogin );
			}
			SendPacket( *client, PacketAckLogin().ToPacket() );
			break;
		default:
			Debug::Log() << "[SERVER] strange packet received with id:  " << intType << Debug::Endl();
			break;
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
	void Server::Receive()
	{
		// Receive packet
		sf::Packet		packet;
		sf::IpAddress	clientIp;
		Port			clientPort;
		sf::UdpSocket::Status	status = m_socket.receive( packet, clientIp, clientPort );

		if ( status == sf::UdpSocket::Done )
		{
			ProcessPacket( clientIp, clientPort, packet );
		}
		else if ( status == sf::UdpSocket::NotReady )
		{
			// do nothing
		}
		else
		{
			Debug::Warning() << "[SERVER] receive error" << Debug::Endl();
		}
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
		Debug::Log() << "[SERVER] logging client " << client.name << " " << client.ipAdress.toString() << "::" << client.port << Debug::Endl();

		return &m_clients[m_clients.size() - 1];
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