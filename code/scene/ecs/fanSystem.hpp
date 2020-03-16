#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entities
	// System must implement GetSignature() and  Run(..) methods
	//==============================================================================================================================================================
	struct System
	{};
}