#pragma once

#include "ecs/fanEcsTag.hpp"

namespace fan
{
	//========================================================================================================
	// scene tags
	//========================================================================================================
	struct TagEditorOnly : EcsTag { ECS_TAG( TagEditorOnly ) };		// entity is not saved
}