#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector2.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Contains the data of the game camera ( what entity it is and how it should be placed )
	//================================================================================================================================	
	struct GameCamera : public EcsSingleton
	{
		ECS_SINGLETON( GameCamera )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		EcsHandle   cameraHandle;
		float		heightFromTarget;
		btVector2	marginRatio;
		float		minOrthoSize;

		static GameCamera& CreateGameCamera( EcsWorld& _world );
		static void        DeleteGameCamera( EcsWorld& _world );
	};
}