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

		enum State{ DISCONNECTED=0, CONNECTED, STARTING };
		enum RandomFlags { MUST_PING_SERVER = 1 << 0, MUST_ACK_START = 1 << 1 };

		EcsWorld		world;
		sf::UdpSocket	socket;
		unsigned short	clientPort			= 53001;
		sf::IpAddress	serverIP			= "127.0.0.1";
		unsigned short	serverPort			= 53000;
		State			state				= DISCONNECTED;
		float			roundTripDelay		= 0.f;
		double			serverLastResponse	= 0.f;	// the last time we received a packet from the server
		float			pingDuration		= .1f;	// server is pinged every X seconds
		float			timeoutDuration		= 3.f;	// server is disconnected after X seconds

		uint32_t	randomFlags;
		double		mustPingServer = -1.f;

	private:
		void	NetworkSend();
		void	NetworkReceive();
	};
}
