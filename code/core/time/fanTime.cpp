#include "fanGlobalIncludes.h"

#include "core/time/fanTime.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Time::Time() :
		  m_renderDelta	 ( 1.f / 60.f )
		, m_logicDelta	 ( 1.f / 60.f )
		, m_physicsDelta ( 1.f / 60.f ) {}

	//================================================================================================================================
	//================================================================================================================================
	std::string Time::SecondsToString( const double _seconds ){
		std::stringstream ss;
		unsigned remainingSeconds = static_cast<unsigned>(_seconds);

		const unsigned hours = remainingSeconds / 3600;
		remainingSeconds -= hours * 3600;
		ss << '[';
		if( hours < 10 ) {
			 ss << '0';
		}
		ss << hours << ':';
		
		const unsigned minuts = remainingSeconds / 60;
		remainingSeconds -= minuts * 60;
		if( minuts < 10 ) {
			ss << 0;
		}
		ss << minuts << ':';

		const unsigned seconds = remainingSeconds;
		if( seconds < 10 ) {
			ss << '0';
		}
		ss << seconds << ']';
		return ss.str();
	}

	//================================================================================================================================
	// Counts the frames during the last second to calculate the framerate
	//================================================================================================================================
	void Time::RegisterFrameDrawn() {
		++ m_framesCounter;

		const float time = ElapsedSinceStartup();
		if ( time - m_lastLogFrameTime > 1.f ) {
			m_realFramerateLastSecond = m_framesCounter;
			m_framesCounter = 0;
			m_lastLogFrameTime = time;
		}
	}


}