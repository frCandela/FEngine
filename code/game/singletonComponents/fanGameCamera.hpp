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
		static void Init( SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

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