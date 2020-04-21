#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct SceneNode;
	class EcsWorld;

	//================================================================================================================================
	// Contains the data of the game camera ( what entity it is and how it should be placed )
	//================================================================================================================================	
	struct GameCamera : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		SceneNode*  cameraNode;
		float		heightFromTarget;
		btVector2	marginRatio;
		float		minOrthoSize;

		static void CreateGameCamera( EcsWorld& _world );
		static void DeleteGameCamera( EcsWorld& _world );
	};
}