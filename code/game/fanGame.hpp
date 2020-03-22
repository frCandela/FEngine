#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/fanGameSerializable.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	//================================================================================================================================
	struct Game
	{
	public:
		enum State { STOPPED, PLAYING, PAUSED };

		Game( const std::string _name );
		~Game();

		void Play();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		Signal< Game& >		onStop;

		std::string	name;
		State		state = State::STOPPED;		
		EcsWorld    world;
	};
}
