#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// Editor camera data (transform, camera, speed, sensitivity )
	//========================================================================================================
	struct EditorCamera : public EcsSingleton
	{
		ECS_SINGLETON( EditorCamera )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		EcsHandle mCameraHandle;
		float     mSpeed           = 10.f;
		float     mSpeedMultiplier = 3.f;
		glm::vec2 mXYSensitivity   = glm::vec2( 0.005f, 0.005f );

		static void Update( EcsWorld& _world, const float _delta );
		static void CreateEditorCamera( EcsWorld& _world );
	};
}