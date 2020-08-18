#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    class GameBase;

	//========================================================================================================
	// Contains game data & a reference to the game client or server depending on which is used
	// allows saving properties of the game into the scene & edition of parameters from the editor
	//========================================================================================================
	struct Game : public EcsSingleton
	{
		ECS_SINGLETON( Game )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		enum State { STOPPED, PLAYING, PAUSED };
        enum Flags { None = 0, Server = 1 };

		State       mState;
		std::string name;
		uint32_t    mFlags = Flags::None;
		PrefabPtr   spaceshipPrefab;

		GameBase* mGame = nullptr;

		bool IsServer() const { return mFlags & Flags::Server; }
	};
}