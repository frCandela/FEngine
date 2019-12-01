#pragma once

#include "fanNetworkCommon.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Client
	{
	public:
		enum State { NONE, CONNECTING, CONNECTED };

		Client( const std::string& _name, const Port _listenPort );
		
		void Update( const float _delta );
		bool Bind();
		void ConnectToServer( const Port _serverPort, const sf::IpAddress _ip );
		void SendToServer( sf::Packet _packet );

	private:
		std::string		m_name;
		Port			m_listenPort;
		sf::UdpSocket   m_socket;
		State			m_state = NONE;

		float m_timer = 0.f;

		Port			m_serverPort;
		sf::IpAddress	m_serverIp = sf::IpAddress::None;

		void Receive();
		void ProcessPacket( sf::Packet& _packet );
	};
}