#include "core/time/fanProfiler.hpp"

namespace fan
{
	//================================================================================================================================
	// Beginning of an interval, saves the times, a name and return the interval unique id
	//================================================================================================================================
	size_t Profiler::OpenTimeInterval( const char _name[ sNameSize ] )
	{
		Interval interval;
		interval.mTime = mClock.Now();
		interval.mID   = mIndex++;

		strcpy_s( interval.mName, _name ); // If you crash here, you probably entered a name that is more than 24 characters long
		mIntervals.push_back( interval );

		return interval.mID;
	}

	//================================================================================================================================
	// End of an interval
	//================================================================================================================================
	void Profiler::CloseTimeInterval( const size_t _index )
	{
		Interval interval;
		interval.mTime = mClock.Now();
		interval.mID   = _index;
		interval.mName[ 0 ] = '\0';
		mIntervals.push_back( interval );
	}

	//================================================================================================================================
	// Clears the profiler
	//================================================================================================================================
	void Profiler::Begin()
	{
		mIntervals.clear();
        mIndex = 0;
		mClock.Reset();
		const size_t index = OpenTimeInterval( "full_interval" );
        (void)index;
        fanAssert( index == 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Profiler::End()
	{

		CloseTimeInterval( 0 );
		mOnProfilingEnd.Emmit();
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedProfile::ScopedProfile( const char  _name[ 24 ] )
	{
        mIndex = Profiler::Get().OpenTimeInterval( _name );
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedProfile::~ScopedProfile()
	{
		Profiler::Get().CloseTimeInterval( mIndex );
	}

}