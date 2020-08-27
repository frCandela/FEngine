#pragma once

#include "core/fanSingleton.hpp"
#include "ecs/fanSignal.hpp"
#include "core/time/fanClock.hpp"

namespace fan
{
	//========================================================================================================
	// Measures time intervals in scoped segments of code
	// data is displayed in the profiler window of the editor
	//========================================================================================================
	class Profiler : public Singleton<Profiler>
	{
	public:

		static const size_t s_nameSize = 32;
		Signal<> onProfilingEnd;

		//================================================================
		//================================================================
		struct Interval
		{

			Clock::TimePoint mTime;
			char             mName[ s_nameSize ];
			size_t           mID;

			bool IsOpening() const { return mName[ 0 ] != '\0'; }
			bool IsClosing() const { return mName[ 0 ] == '\0'; }
		};

		size_t	OpenTimeInterval( const char _name[ s_nameSize ] );
		void	CloseTimeInterval( const size_t _index );

		void Begin();
		void End();
		inline const std::vector<Interval>& GetIntervals() { return mIntervals; }

	private:
		Clock                           mClock;
		size_t                          mIndex = 0;
		std::vector<Profiler::Interval> mIntervals;
	};

	//================================================================================================================================
	// Calls Profiler::Open and Profiler::Close in its constructor & destructor
	// Use the SCOPED_PROFILE macro to allow removal of symbols when profiling is not used
	//================================================================================================================================
	class ScopedProfile
	{
	public:
		ScopedProfile( const char  _name[ Profiler::s_nameSize ] );
		~ScopedProfile();

	private:
		size_t mIndex;
	};
#define SCOPED_PROFILE( _name ) ScopedProfile _name(#_name);
}

