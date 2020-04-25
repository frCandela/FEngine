#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "network/fanUdpSocket.hpp"

namespace fan
{
	struct Game;
	struct ClientNetworkManager;

	//================================================================================================================================
	//================================================================================================================================
	struct GameClient
	{
		GameClient( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		EcsWorld			  world;
		Game*				  game;
		ClientNetworkManager* netManager;
	};
}
