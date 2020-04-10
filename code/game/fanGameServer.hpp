#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct Client
	{
		enum State{ 
			DISCONNECTED,		// client is disconnected
			CONNECTED_NEED_ACK, // client is connected but needs an ACK
			CONNECTED			// client is connected
		};

		sf::IpAddress	ip;
		unsigned short	port;
		std::string		name = "";
		State			state = State::DISCONNECTED;
		float			ping = 0.f;
		float			lastResponse = 0.f;
	};

	//================================================================================================================================
	//
	//================================================================================================================================
	struct GameServer
	{
		GameServer( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		EcsWorld			world;
		sf::UdpSocket		socket;
		unsigned short		serverPort = 53000;
		std::vector<Client> clients;

	private:
		void	NetworkSend();
		void	NetworkReceive();

		Client* FindClient( const sf::IpAddress _ip, const unsigned short _port );
	};					  
}
