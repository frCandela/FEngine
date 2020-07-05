#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/shapes/fanAABB.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// axis aligned bounding box
	//==============================================================================================================================================================
	struct Bounds : public EcsComponent
	{
		ECS_COMPONENT( Bounds )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		AABB aabb;
	};
	static constexpr size_t sizeof_bounds = sizeof( Bounds );
}