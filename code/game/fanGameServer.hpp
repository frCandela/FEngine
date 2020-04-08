#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
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

		EcsWorld    world;
	};
}
