#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "game/fanGameSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	//================================================================================================================================
	struct Game
	{
	public:
		enum State { STOPPED, PLAYING, PAUSED };

		Game( const std::string _name, void ( *_initializeTypesEcsWorld )( EcsWorld& ) );
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
		Scene&		scene;
	};
}
