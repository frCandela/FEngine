#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct SceneNode;
	class EcsWorld;

	//================================================================================================================================
	//================================================================================================================================	
	struct GameCamera : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		//ComponentPtr < Camera >		 m_camera;
		//ComponentPtr<PlayersManager> m_playersManager;
		SceneNode*  cameraNode;
		float		heightFromTarget;
		btVector2	marginRatio;
		float		minOrthoSize;

		static void CreateGameCamera( EcsWorld& _world );
		static void DeleteGameCamera( EcsWorld& _world );
	};
}