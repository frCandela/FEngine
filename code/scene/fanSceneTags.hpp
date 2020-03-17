#pragma once

#include "ecs/fanTag.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// scene tags
	//==============================================================================================================================================================
	struct tag_editorOnly : Tag { DECLARE_TAG() };
	struct tag_boundsOutdated : Tag { DECLARE_TAG() };	// Updates even when the scene is not playing
}