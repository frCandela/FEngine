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
		FrameIndex	frameIndex;			// the index of the current time
		FrameIndex	frameStart;			// the index of the first frame of the game
		float		logicDelta;			// time between two frames in seconds
		float		timeScaleDelta;		// accelerate, decelerates the logic frame rate
		float		timeScaleIncrement; // the maximum amount that can be added to each frame

		// @hack for the editor, only one of these should be null
		GameClient* gameClient = nullptr;
		GameServer* gameServer = nullptr;

		bool IsServer() const { return gameServer != nullptr; }
		static EcsHandle SpawnSpaceship( EcsWorld& _world, const bool _hasPlayerInput, const bool _hasPlayerController );
		void OnShiftFrameIndex( const int _framesDelta );
	};
}