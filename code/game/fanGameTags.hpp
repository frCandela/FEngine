#pragma once

#include "ecs/fanEcsTag.hpp"

namespace fan
{
	//========================================================================================================
	// game tags
	//========================================================================================================

	// a particle is destroyed when out of the sunlight
	struct tag_sunlight_occlusion : EcsTag { ECS_TAG( tag_sunlight_occlusion ) };
}