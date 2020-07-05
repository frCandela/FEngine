#pragma once

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct GameClient
	{
		GameClient( const std::string _name );

		void Start();
		void Stop();
		void Step( const float _delta );
		EcsWorld			  world;
		void RollbackResimulate();

		static void CreateGameAxes();
	};
}
