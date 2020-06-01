#pragma once

#include "network/fanPacket.hpp"

namespace fan
{
	//================================================================================================================================
	// A classic Udp socket 
	//================================================================================================================================
	class UdpSocket
	{
	public:
		UdpSocket();
		using Status = sf::Socket::Status;

		static constexpr size_t maxPacketSize = 508;

		Status	Bind( unsigned short _port, const IpAddress& _address = IpAddress::Any ) { return m_socket.bind( _port, _address ); }
		void	Unbind() { m_socket.unbind(); }
		unsigned short GetPort() const { return m_socket.getLocalPort(); }		
		Status	Receive( Packet& _packet, IpAddress& _remoteAddress, unsigned short& _remotePort );
 		Status	Send( Packet& _packet, const IpAddress& _remoteAddress, unsigned short _remotePort ) { return m_socket.send( _packet.ToSfml(), _remoteAddress, _remotePort ); }

	private:
		sf::UdpSocket		m_socket;
	};
}