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
		enum State{ NONE, CONNECTED };

		sf::IpAddress	ip;
		unsigned short	port;
		std::string		name;
		State			state = State::NONE;
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
		unsigned short		listenPort = 53000;
		std::vector<Client> clients;
	};
}
