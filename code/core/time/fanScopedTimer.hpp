#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
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