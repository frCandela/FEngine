#pragma once

#include <string>

namespace fan
{
	//================================================================================================================================
	// counts elapsed time in a scope and prints it out to the console
	//================================================================================================================================
	class ScopedTimer
	{
	public:
		ScopedTimer( const std::string _label );
		~ScopedTimer();

	private:
		double		m_startValue;
		std::string m_label;
	};
}