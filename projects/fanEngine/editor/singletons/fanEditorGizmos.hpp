#pragma once

#include <map>
#include "LinearMath/btVector3.h"
#include "LinearMath/btTransform.h"

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
	class EcsWorld;
	struct FxTransform;

	//========================================================================================================
	// allows displaying of the translation manipulator
	// @todo make this a singleton component in the editor ecs world
	//========================================================================================================
	struct EditorGizmos : EcsSingleton
	{
		ECS_SINGLETON( EditorGizmos )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

		//================================================================
		//================================================================	
		struct GizmoCacheData
		{
			int axisIndex;
			bool pressed = false;
			Vector3 offset;
		};
		bool DrawMoveGizmo( const FxTransform& _transform, const size_t _uniqueID, Vector3& _newPosition );

		std::map< size_t, GizmoCacheData >  mGizmoCacheData;
		EcsWorld*                           mWorld;
	};
}