#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"
#include "core/math/shapes/fanAABB.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Bounds : public ecComponent
	{
		DECLARE_COMPONENT( Bounds )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );

		AABB aabb;
	};
	static constexpr size_t sizeof_bounds = sizeof( Bounds );
}