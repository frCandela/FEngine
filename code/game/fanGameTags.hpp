#pragma once

#include "ecs/fanEcsTag.hpp"

namespace fan
{
	//========================================================================================================
	// game tags
	//========================================================================================================

	// a particle is destroyed when out of the sunlight
	struct TagSunlightOcclusion : EcsTag { ECS_TAG( TagSunlightOcclusion ) };
}