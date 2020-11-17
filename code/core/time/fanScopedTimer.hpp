#pragma once

#include <string>
#include "core/time/fanClock.hpp"

namespace fan
{
	//========================================================================================================
	// counts elapsed time in a scope and prints it out to the console
	//========================================================================================================
	class ScopedTimer
	{
	public:
		ScopedTimer( const std::string _label );
		~ScopedTimer();

	private:
        Clock               mClock;
		std::string         mLabel;
	};
}