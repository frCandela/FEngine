#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
	// Compile time power for integers
	//================================================================================================================================
	static constexpr int S_Pow( int _value, int _power ) {
		int result = _value;
		for (int index = 1; index < _power; index++)
		{
			result *= _value;
		}
		return result;
	}
}
