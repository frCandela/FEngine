#pragma once

#include <chrono>

namespace fan
{
	//========================================================================================================
	// Clock for counting time
	// starts when constructed
	//========================================================================================================
	class Clock
	{
	public:
		using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using Seconds = std::chrono::seconds;
		using Microseconds = std::chrono::microseconds;

		Clock();
		void		Reset();
		TimePoint	Now() const;
		float		ElapsedSeconds() const;		

		void operator+=( const Microseconds& _seconds ){
            mStartPoint += _seconds;
		}

		static float SecondsBetween( const TimePoint& _t1, const TimePoint& _t2 )
		{
			return std::chrono::duration<float>( _t2 - _t1 ).count();
		}

	private:
		std::chrono::high_resolution_clock                          mClock;
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartPoint;
	};
}