#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "network/fanUDPSocket.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct Client
	{
		enum State{ 
			  CONNECTING			// client is disconnected
			, CONNECTED_NEED_ACK	// client is connected but needs an ACK
			, CONNECTED				// client is connected
			, STARTING				// client is ready to start the game
			, PLAYING				// client is playing the game
		};

		sf::IpAddress	ip;
		unsigned short	port;
		std::string		name = "";
		State			state = State::CONNECTING;
		float			roundTripDelay = 0.f;
		double			lastResponse = 0.f;
		double			lastPingTime = 0.f;
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

		enum State { WAITING_FOR_PLAYERS = 0, STARTING, PLAYING };

		EcsWorld			world;
		UdpSocket			socket;
		unsigned short		serverPort = 53000;
		std::vector<Client> clients;
		State				state = WAITING_FOR_PLAYERS;

		float pingDuration = .1f;	 // clients are pinged every X seconds
		float timeoutDuration = 3.f; // clients are disconnected after X seconds
	private:
		void	NetworkSend();
		void	NetworkReceive();

		Client* FindClient( const sf::IpAddress _ip, const unsigned short _port );
	};					  
}
