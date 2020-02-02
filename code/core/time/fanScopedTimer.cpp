#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanTime.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	ScopedTimer::ScopedTimer( const std::string _label ) 
		: m_label( _label ) 
	{
		m_startValue = Time::ElapsedSinceStartupDouble();
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedTimer::~ScopedTimer() {
		const double stopValue = Time::ElapsedSinceStartupDouble();

		Debug::Get() << Debug::Severity::highlight << "Scoped timer - " << m_label << " - took: " << (stopValue - m_startValue) * 1000 << " ms" << Debug::Endl();
	}
}