#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector2.hpp"

namespace fan
{
	struct SceneNode;
	struct Transform;
	struct Camera;
	class EcsWorld;

	//================================================================================================================================
	// Editor camera data (transform, camera, speed, sensitivity )
	//================================================================================================================================
	struct EditorCamera : public EcsSingleton
	{
		ECS_SINGLETON( EditorCamera )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		SceneNode* cameraNode;
		Transform* transform;
		Camera* camera;

		float	  speed = 10.f;
		float	  speedMultiplier = 3.f;
		btVector2 xySensitivity = btVector2( 0.005f, 0.005f );

		static void Update( EditorCamera& camera, const float _delta );
		static void CreateEditorCamera( EcsWorld& _world );
	};
}