#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	struct TimePoint
	{
		using TimePointImpl = std::chrono::time_point<std::chrono::high_resolution_clock>;

		TimePoint( TimePointImpl _timepoint = TimePointImpl() ) : m_timePoint( _timepoint ) {}

		static float SecondsBetween( const TimePoint& _t1, const TimePoint& _t2 )
		{
			return std::chrono::duration<float>( _t2.m_timePoint - _t1.m_timePoint ).count();
		}

	private:
		TimePointImpl m_timePoint;
	};

	//================================================================================================================================
	//================================================================================================================================
	class Clock
	{
	public:
		Clock();
		void		Reset();
		TimePoint	Now() const;
		float		ElapsedSeconds() const;

	private:
		std::chrono::high_resolution_clock m_clock;
		TimePoint m_startPoint;
	};

}