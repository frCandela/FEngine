#pragma once

#include "ecs/fanEcsWorld.hpp"

#include "network/fanUDPSocket.hpp"

namespace fan
{
	struct ServerNetworkManager;
	struct Game;

	//================================================================================================================================
	//================================================================================================================================
	struct GameServer
	{
		GameServer( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		EcsWorld			  world;
		ServerNetworkManager* netManager;
		Game*				  game;
	};					  
}
