#include "fanGlobalIncludes.h"

#include "core/fanTime.h"

namespace fan {
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
}