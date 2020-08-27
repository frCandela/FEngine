#include "core/time/fanScopedTimer.hpp"

#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	ScopedTimer::ScopedTimer( const std::string _label )
		: mLabel( _label )
	{
        mStartValue = Time::ElapsedSinceStartup();
	}

	//========================================================================================================
	//========================================================================================================
	ScopedTimer::~ScopedTimer()
	{
		const double stopValue = Time::ElapsedSinceStartup();

		Debug::Highlight()  << "Scoped timer - " << mLabel
		                    << " - took: " << ( stopValue - mStartValue ) * 1000 << " ms"
		                    << Debug::Endl();
	}
}