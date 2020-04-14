#pragma once

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

namespace fan
{
	using IpAddress = sf::IpAddress;
	using Packet = sf::Packet;

	//================================================================================================================================
	// A classic udp socket with a reliability layer on top of it
	//================================================================================================================================
	class UdpSocket
	{
	public:
		UdpSocket();

		using Status = sf::Socket::Status;

		void	SetBlocking( const bool _blocking ) { socket.setBlocking( _blocking ); }
		Status	Bind( unsigned short _port, const IpAddress& _address = IpAddress::Any ) { return socket.bind( _port, _address ); }
		void	Unbind() { socket.unbind(); }
		Status  Receive( Packet& _packet, IpAddress& _remoteAddress, unsigned short& _remotePort ) { return socket.receive( _packet, _remoteAddress, _remotePort ); }
		Status  Send( Packet& _packet, const IpAddress& _remoteAddress, unsigned short _remotePort ) { return socket.send( _packet, _remoteAddress, _remotePort ); }
	private:
		sf::UdpSocket	socket;
	};
}