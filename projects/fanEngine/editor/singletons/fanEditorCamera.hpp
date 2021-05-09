#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/math/fanFixedPoint.hpp"

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
		Fixed     mSpeed;
		Fixed     mSpeedMultiplier;
		glm::vec2 mXYSensitivity ;

		static void Update( EcsWorld& _world, const Fixed _delta );
		static void CreateEditorCamera( EcsWorld& _world );
	};
}