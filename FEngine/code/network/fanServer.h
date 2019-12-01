#pragma once

#include "network/fanNetworkCommon.h"

namespace fan
{
	struct PacketLogin;

	//================================================================================================================================
	//================================================================================================================================
	class Server
	{
	public:
		enum State {		NONE, WAITING_PLAYERS };
		enum ClientState {  WAITING, CONNECTED };

		struct ClientData
		{
			sf::IpAddress	ipAdress;
			Port			port;
			std::string		name;
			ClientState		state = ClientState::WAITING;
		};

		Server( const Port _listenPort );

		void Update( const float _delta );
		bool Bind();
		void SendPacket( const ClientData& _client,  sf::Packet _packet );

		const std::vector<ClientData >& GetClients() const { return m_clients; }

	private:
		Port			m_listenPort;
		sf::UdpSocket   m_socket;
		State			m_state = NONE;

		std::vector<ClientData > m_clients;

		void Receive();
		void ProcessPacket( const sf::IpAddress _ip, const Port _port, sf::Packet& _packet );		

		ClientData * FindClient( const sf::IpAddress _ip, const Port _port  );
		ClientData * AddClient( const sf::IpAddress _ip, const Port _port, const PacketLogin& _loginInfo  );
	};
}