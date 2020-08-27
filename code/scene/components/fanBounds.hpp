#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/shapes/fanAABB.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct Bounds : public EcsComponent
	{
		ECS_COMPONENT( Bounds )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		AABB mAabb;
	};
}