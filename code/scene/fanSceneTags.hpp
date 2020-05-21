#pragma once

#include "ecs/fanEcsTag.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// scene tags
	//==============================================================================================================================================================
	struct tag_editorOnly	  : EcsTag { DECLARE_TAG( tag_editorOnly ) };		// entity is not saved 
	struct tag_boundsOutdated : EcsTag { DECLARE_TAG( tag_boundsOutdated ) };	// bounds need to be recomputed
}