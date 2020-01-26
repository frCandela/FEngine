#include "network/fanUDPSocket.hpp"
#include "network/packets/fanPacketPing.hpp"
#include "network/packets/fanPacketLogin.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	UDPSocket::UDPSocket()
	{
		m_socket.setBlocking( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UDPSocket::Create( const std::string& _name, const Port _listenPort )
	{
		m_name = _name;
		m_listenPort = _listenPort;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UDPSocket::Bind()
	{
		if ( m_socket.bind( m_listenPort ) != sf::Socket::Done )
		{
			Debug::Warning() << m_name << " bind failed on port " << m_listenPort << Debug::Endl();
			return false;
		}
		Debug::Log() << m_name << " bind success on port " << m_listenPort << Debug::Endl();
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UDPSocket::UnBind()
	{
		m_socket.unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UDPSocket::Send( sf::Packet _packet, const sf::IpAddress& _remoteAddress, unsigned short _remotePort )
	{
		const sf::Socket::Status status = m_socket.send( _packet, _remoteAddress, _remotePort );
		if ( status != sf::Socket::Done )
		{
			Debug::Warning() << m_name << " send error : " << status << Debug::Endl();
			return false;
		}
		return true;
	}
	   
	//================================================================================================================================
	//================================================================================================================================
	bool UDPSocket::Receive( sf::Packet& _packet, bool& _disconnected )
	{
		// Receive packet
		_disconnected = false;
		_packet.clear();
		sf::UdpSocket::Status	status = m_socket.receive( _packet, m_lastReceiveIp, m_lastReceivePort );

		if ( status == sf::UdpSocket::Done )
		{
			if ( m_portWhiteList == 0 || ( m_lastReceiveIp == m_ipWhiteList && m_lastReceivePort == m_portWhiteList) )
			{
				return true;
			}			
		}
		else if ( status == sf::UdpSocket::NotReady )
		{
			// do nothing
		}
		else if ( status == sf::UdpSocket::Disconnected )
		{
			_disconnected = true;
			return true;
		}
		else
		{
			Debug::Warning() << m_name << " receive error" << Debug::Endl();
		}
		return false;
	}
}