#pragma once

#include "ecs/fanSingletonComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct GameClient;
	struct GameServer;

	//================================================================================================================================
	// Contains game data & a reference to the game client or server depending on which is used
	// allows saving properties of the game into the scene & edition of parameters from the editor
	//================================================================================================================================	
	struct Game : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		enum State { STOPPED, PLAYING, PAUSED };

		State	    state;
		std::string name;
		PrefabPtr	spaceshipPrefab;
		FrameIndex	frameIndex;			// the index of the current time
		FrameIndex	frameStart;			// the index of the first frame of the game
		float		logicDelta;			// time between two frames in seconds

		// @hack for the editor, only one of these should be null
		GameClient* gameClient = nullptr;
		GameServer* gameServer = nullptr;

		static EntityHandle SpawnSpaceship( EcsWorld& _world );
		void OnShiftFrameIndex( const int _framesDelta );
	};
}