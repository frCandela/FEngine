#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct GameClient;
	struct GameServer;

	//================================================================================================================================
	// Contains game data & a reference to the game client or server depending on which is used
	// allows saving properties of the game into the scene & edition of parameters from the editor
	// @todo, put time related stuff in a singleton in the scene lib
	//================================================================================================================================	
	struct Game : public EcsSingleton
	{
		ECS_SINGLETON( Game )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		enum State { STOPPED, PLAYING, PAUSED };

		State	    state;
		std::string name;
		PrefabPtr	spaceshipPrefab;

		// @hack for the editor, only one of these should be null
		GameClient* gameClient = nullptr;
		GameServer* gameServer = nullptr;

		bool IsServer() const { return gameServer != nullptr; }
		static EcsHandle SpawnSpaceship( EcsWorld& _world, const bool _hasPlayerInput, const bool _hasPlayerController );
	};
}