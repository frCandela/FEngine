#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

namespace fan
{
	//================================================================================================================================
	// 
	//================================================================================================================================
	struct GameClient
	{
		GameClient( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		enum Status{ DISCONNECTED=0, CONNECTED=1 };

		EcsWorld		world;
		sf::UdpSocket	socket;
		unsigned short	clientPort			= 53001;
		sf::IpAddress	serverIP			= "127.0.0.1";
		unsigned short	serverPort			= 53000;
		Status			status				= DISCONNECTED;
		float			roundTripDelay		= 0.f;
		double			serverLastResponse	= 0.f;	// the last time we received a packet from the server
		float			pingDuration		= .1f;	// server is pinged every X seconds
		float			timeoutDuration		= 3.f;	// server is disconnected after X seconds

		double mustPingServer = -1.f;
		float ping = 0.f;

	private:
		void	NetworkSend();
		void	NetworkReceive();
	};
}
