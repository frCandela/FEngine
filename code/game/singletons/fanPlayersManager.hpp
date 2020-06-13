#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	// @wip old code from the gameobject system
	//================================================================================================================================	
	struct  PlayersManager : public EcsSingleton
	{
		ECS_SINGLETON( PlayersManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		//================================================================	
		//================================================================
		struct PlayerData
		{
			SceneNode * spaceship = nullptr;
			SceneNode * persistent = nullptr;
		};

// 		Signal< Gameobject* > onAddPlayer;
		PrefabPtr						 playerPrefab;
		std::map< uint32_t, PlayerData > players;

// 		void AddPlayer( const int _ID, const std::string& _name );
// 		void SpawnSpaceShips();
// 		void RemovePlayer( const int _ID );
// 		void OnJoystickConnect( int _joystickID, bool _connected );
// 		void OnPlayerDie( Gameobject* _gameobject );
	};
}