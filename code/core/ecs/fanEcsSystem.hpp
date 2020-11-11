#pragma once

#include "fanEcsTypes.hpp"
#include "fanEcsView.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entities
	// implement GetSignature() and  Run(..) methods
	//==============================================================================================================================================================
	struct EcsSystem {};
}