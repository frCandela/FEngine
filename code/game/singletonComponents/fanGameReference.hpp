#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct Game;

	//================================================================================================================================
	// a reference to the game
	// allows saving properties of the game into the scene & edition of parameters from the editor
	// you normally don't need it for gameplay code (we will see how this goes :3 )
	//================================================================================================================================	
	struct GameReference : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		Game* const game = nullptr;
	};
}