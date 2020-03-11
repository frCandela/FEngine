#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entites
	//
	// System must call the (DECLARE/REGISTER)_SYSTEM macro and implement Clear() method that set its signature.
	// It also must implement a static Run(..) method that runs its logic.
	// It also must be registered in the EcsWorld constructor to be assigned a unique signature
	//==============================================================================================================================================================
	struct System
	{};
}