#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	// THE GAME !
	//================================================================================================================================
	struct Game
	{
	public:
		enum State { STOPPED, PLAYING, PAUSED };

		Game( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		std::string	name;
		State		state = State::STOPPED;		
		EcsWorld    world;

		PrefabPtr spaceship;
	};
}
