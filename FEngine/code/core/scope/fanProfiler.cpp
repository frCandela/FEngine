#include "fanGlobalIncludes.h"
#include "core/scope/fanProfiler.h"

#include "core/fanTime.h"
#include <string.h>
namespace fan {
	//================================================================================================================================
	// Beginning of an interval, saves the times, a name and return the interval unique id
	//================================================================================================================================
	size_t Profiler::OpenTimeInterval( const char _name[s_nameSize] ) {
		Interval interval;		
		interval.time = Time::ElapsedSinceStartupDouble();
		interval.id = m_index ++;

		strcpy_s( interval.name, _name ); // If you crash here, you probably entered a name that is more than 16 characters long
		m_intervals.push_back( interval );

		return interval.id;
	}

	//================================================================================================================================
	// End of an interval
	//================================================================================================================================
	void Profiler::CloseTimeInterval( const size_t _index ) {
		Interval interval;
		interval.time = Time::ElapsedSinceStartupDouble();
		interval.id = _index;
		interval.name[0] = '\0';
		m_intervals.push_back(interval);
	}

	//================================================================================================================================
	// Clears the profiler
	//================================================================================================================================
	void Profiler::Begin() {
		m_intervals.clear(); 
		m_index	= 0;
		const size_t index = OpenTimeInterval("full_interval");
		assert( index  == 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Profiler::End() {

		CloseTimeInterval( 0 );
		onProfilingEnd.Emmit();
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedProfile::ScopedProfile( const char  _name[16] ) {
		m_index = Profiler::Get().OpenTimeInterval( _name );
	}

	//================================================================================================================================
	//================================================================================================================================
	ScopedProfile::~ScopedProfile() {
		Profiler::Get().CloseTimeInterval( m_index );
	}
	
}