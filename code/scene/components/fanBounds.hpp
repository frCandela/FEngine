#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "core/math/shapes/fanAABB.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Bounds : public Component
	{
		DECLARE_COMPONENT( Bounds )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );

		AABB aabb;
	};
	static constexpr size_t sizeof_bounds = sizeof( Bounds );
}