#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	// Some global game data
	//========================================================================================================
	struct Game : public EcsSingleton
	{
		ECS_SINGLETON( Game )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		bool      mIsServer = false;
		PrefabPtr mSpaceshipPrefab;
	};
}