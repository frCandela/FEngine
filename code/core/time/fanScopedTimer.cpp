#include "core/time/fanScopedTimer.hpp"

#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ScopedTimer::ScopedTimer( const std::string _label )
		: m_label( _label )
	{
		m_startValue = Time::ElapsedSinceStartup();
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedTimer::~ScopedTimer()
	{
		const double stopValue = Time::ElapsedSinceStartup();

		Debug::Get() << Debug::Severity::highlight << "Scoped timer - " << m_label << " - took: " << ( stopValue - m_startValue ) * 1000 << " ms" << Debug::Endl();
	}
}