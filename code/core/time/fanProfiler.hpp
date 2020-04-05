#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/fanSingleton.hpp"
#include "core/time/fanClock.hpp"

namespace fan
{
	//================================================================================================================================
	// Measures time intervals in scoped segments of code
	// data is displayed in the profiler window of the editor
	//================================================================================================================================
	class Profiler : public Singleton<Profiler>
	{
	public:

		static const size_t s_nameSize = 16;
		Signal<> onProfilingEnd;

		//================================================================
		//================================================================
		struct Interval
		{

			TimePoint time;
			char	name[ s_nameSize ];
			size_t  id;

			bool IsOpening() const { return name[ 0 ] != '\0'; }
			bool IsClosing() const { return name[ 0 ] == '\0'; }
		};

		size_t	OpenTimeInterval( const char _name[ s_nameSize ] );
		void	CloseTimeInterval( const size_t _index );

		void Begin();
		void End();
		inline const std::vector<Interval>& GetIntervals() { return m_intervals; }

	private:
		Clock m_clock;
		size_t m_index = 0;
		std::vector<Profiler::Interval> m_intervals;
	};

	//================================================================================================================================
	// Calls Profiler::Open and Profiler::Close in its constructor & destructor
	// Use the SCOPED_PROFILE macro to allow removal of symbols when profiling is not used
	//================================================================================================================================
	class ScopedProfile
	{
	public:
		ScopedProfile( const char  _name[ 16 ] );
		~ScopedProfile();
	private:
		size_t m_index;
	};
#define SCOPED_PROFILE( _name ) ScopedProfile _name(#_name);
}

