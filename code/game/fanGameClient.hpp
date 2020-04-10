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
		unsigned short	listenPort = 53001;
		sf::IpAddress	serverIP = "127.0.0.1";
		unsigned short	serverPort = 53000;
		Status			status;

	private:
		void	NetworkSend();
		void	NetworkReceive();
	};
}
