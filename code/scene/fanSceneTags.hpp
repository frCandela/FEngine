#pragma once

#include "ecs/fanTag.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// scene tags
	//==============================================================================================================================================================
	struct tag_editorOnly : Tag { DECLARE_TAG() };		// entity is not saved 
	struct tag_boundsOutdated : Tag { DECLARE_TAG() };	// bounds need to be recomputed
}