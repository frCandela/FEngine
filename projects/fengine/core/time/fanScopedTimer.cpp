#include "core/time/fanScopedTimer.hpp"

#include "core/fanDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	ScopedTimer::ScopedTimer( const std::string _label )
		: mLabel( _label )
	{
        mClock.Reset();
	}

	//========================================================================================================
	//========================================================================================================
	ScopedTimer::~ScopedTimer()
	{
		const double elapsedSeconds = mClock.ElapsedSeconds();

		Debug::Highlight()  << "Scoped timer - " << mLabel
		                    << " - took: " << elapsedSeconds * 1000 << " ms"
		                    << Debug::Endl();
	}
}