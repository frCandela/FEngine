#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct SceneNode;
	struct Transform;
	struct Camera;

	//================================================================================================================================
	//================================================================================================================================
	struct EditorCamera : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		SceneNode* node;
		Transform* transform;
		Camera* camera;

		float	  speed = 10.f;
		float	  speedMultiplier = 3.f;
		btVector2 xySensitivity = btVector2( 0.005f, 0.005f );

		static void Update( EditorCamera& camera, const float _delta );
	};
}