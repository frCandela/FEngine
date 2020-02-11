#include "core/time/fanClock.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Clock::Clock()
	{
		m_startPoint = m_clock.now();
	}

	//================================================================================================================================
	//================================================================================================================================
	TimePoint Clock::Now() const
	{
		return TimePoint( m_clock.now() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Clock::Reset()
	{
		m_clock = std::chrono::high_resolution_clock();
		m_startPoint = m_clock.now();
	}

	//================================================================================================================================
	//================================================================================================================================
	float Clock::ElapsedSeconds() const
	{
		return TimePoint::SecondsBetween( m_startPoint, Now() );
	}
}