#include "fanGlobalIncludes.h"
#include "network/fanClient.h"

#include "network/packets/fanPacketPing.h"
#include "network/packets/fanPacketLogin.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Client::Client() 
	{
		m_socket.setBlocking( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::Create( const std::string& _name, const Port _listenPort )
	{
		m_name = _name;
		m_listenPort = _listenPort;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Client::Bind()
	{
		if ( m_socket.bind( m_listenPort ) != sf::Socket::Done )
		{
			Debug::Warning() << "[CLIENT] " << m_name << ": bind failed on port " << m_listenPort << Debug::Endl();
			return false;
		}
		Debug::Log() << "[CLIENT] " << m_name << ": bind success on port " << m_listenPort << Debug::Endl();
		return true;				
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::UnBind()
	{
		m_socket.unbind();
	}	

	//================================================================================================================================
	//================================================================================================================================
	void Client::ConnectToServer( const Port _serverPort, const sf::IpAddress _serverIp )
	{
		m_serverPort = _serverPort;
		m_serverIp = _serverIp;

		m_state = CONNECTING;
		m_timer = 0.5f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::Update( const float _delta )
	{
		if ( m_state != State::NONE )
		{
			Receive();
		}

		m_timer -= _delta;

		switch ( m_state )
		{
		case State::NONE:
			break;
		case State::CONNECTING:
			if ( m_timer < 0.f )
			{
				Debug::Log() << "[CLIENT] " << m_name << ": attempting connection to server" << Debug::Endl();
				PacketLogin packet( m_name );
				SendToServer( packet.ToPacket() );
				m_timer = 0.5f;
			}
			break;
		case State::CONNECTED:
			break;

		default:
			break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::SendToServer( sf::Packet _packet )
	{
		const sf::Socket::Status status = m_socket.send( _packet, m_serverIp, m_serverPort );
		if ( status != sf::Socket::Done )
		{
			Debug::Warning() << "[CLIENT] " << m_name << ": send error" << Debug::Endl(); 
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::ProcessPacket( sf::Packet& _packet )
	{
		// Process packet
 		sf::Uint16 intType;
 		_packet >> intType;
		const PacketType type = PacketType(intType);		

		switch ( type )
		{
		case PacketType::ACK_LOGIN : 
			Debug::Log() << "[CLIENT] " << m_name << ": connected !"  << Debug::Endl();
			m_state = State::CONNECTED;
			break;
		case PacketType::PING:			
			SendToServer(_packet);
			break;
		case PacketType::START_GAME:
			Debug::Log() << "[CLIENT] " << m_name << ": start game " << Debug::Endl();
			break;
		default:
			Debug::Warning() << "[CLIENT] " << m_name << ": strange packet received with id: " << intType << Debug::Endl();
			break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Client::Receive()
	{
		// Receive packet
		sf::Packet packet;
		sf::IpAddress	sender;
		Port	port;
		sf::UdpSocket::Status	status = m_socket.receive( packet, sender, port );

		if ( status == sf::UdpSocket::Done )
		{
			if ( sender == m_serverIp && port == m_serverPort )
			{
				ProcessPacket(packet);
			}
		}
		else if ( status == sf::UdpSocket::NotReady || status == sf::UdpSocket::Disconnected )
		{
			// do nothing
		}
		else
		{
			Debug::Warning() << "[CLIENT] " << m_name << ": receive error" << Debug::Endl();
		}
	}
}