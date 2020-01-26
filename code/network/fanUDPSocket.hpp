#pragma once

#include "fanNetworkPrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class UDPSocket
	{
	public:		
		UDPSocket( );

		void Create( const std::string& _name, const Port _listenPort );		
		bool Receive( sf::Packet& _packet, bool& _disconnected );

		bool Bind();
		void UnBind();
		Port GetPort() const { return m_listenPort; }
		void SetPort( const Port _port ) { m_listenPort = _port; }
		inline const std::string& GetName() const { return m_name; }
		void OnlyReceiveFrom( sf::IpAddress	_ip, Port _port ) { m_ipWhiteList = _ip; m_portWhiteList = _port; }
		bool Send( sf::Packet _packet, const sf::IpAddress& _remoteAddress, unsigned short _remotePort);

		const sf::IpAddress	& GetLastReceiveIp() const	 { return m_lastReceiveIp;}
		const Port			& GetLastReceivePort() const { return m_lastReceivePort;}

	private:
		std::string		m_name;
		Port			m_listenPort;
		sf::UdpSocket   m_socket;

		sf::IpAddress	m_lastReceiveIp;
		Port			m_lastReceivePort;

		sf::IpAddress	m_ipWhiteList;
		Port			m_portWhiteList = 0; // If port == 0 -> no whitelist
	};
}