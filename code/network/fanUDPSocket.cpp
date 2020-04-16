#include "fanUDPSocket.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	UdpSocket::UdpSocket()
	{
		m_socket.setBlocking( false );
	}


	//================================================================================================================================
	//================================================================================================================================
	UdpSocket::Status	UdpSocket::Receive( Packet& _packet, IpAddress& _remoteAddress, unsigned short& _remotePort )
	{ 
		const Status status = m_socket.receive( _packet.ToSfml(), _remoteAddress, _remotePort );
		
		// Read tag
		if( status == sf::UdpSocket::Done )
		{
			_packet >> _packet.tag;
		}

		return status;
	}
}