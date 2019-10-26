#include "fanGlobalIncludes.h"
#include "core/time/fanScopedTimer.h"
#include "core/time/fanTime.h"

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

		Debug::Get() << Debug::Severity::highlight << "Scoped timer - " << m_label << " - took: " << stopValue - m_startValue << " seconds" << Debug::Endl();
	}
}