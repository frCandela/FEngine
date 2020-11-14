#include "core/fanDebug.hpp"

#include <iostream>
#include "glfw/glfw3.h"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Debug::Debug()
	{
        mCurrentSeverity = Severity::log;
        mCurrentType     = Type::other;
	}

	//========================================================================================================
	//========================================================================================================
	void Debug::Flush()
	{
		// Push the log into the buffer
		LogItem item;
		item.message = mStringstream.str();
		item.severity = mCurrentSeverity;
		item.type = mCurrentType;
		item.time = glfwGetTime();
		mLogBuffer.push_back( item );

		// stdio
		std::cout << Debug::SecondsToString( item.time );
		switch ( mCurrentSeverity )
		{
		case Severity::log:
		{
			std::cout << "[LOG]";
		} break;
		case Severity::warning:
		{
			std::cout << "[WARNING]";
		} break;
		case Severity::error:
		{
			std::cout << "[ERROR]";
		} break;
		case Severity::highlight:
		{
			std::cout << "[HIGH]";
		} break;
		default:
            fanAssert( false );
			break;
		}
		std::cout << " " << mStringstream.str().c_str() << std::endl;
		mStringstream.str( "" ); // clear
		mCurrentSeverity = Severity::log;
        mCurrentType     = Type::other;
		onNewLog.Emmit( item );
	}

	//========================================================================================================
	//========================================================================================================
	void  Debug::Log( const std::string _message )
	{
	    Get() << Severity::log << _message << Debug::Endl();
	}

    //========================================================================================================
    //========================================================================================================
	void  Debug::Warning( const std::string _message )
	{
	    Get() << Severity::warning << _message << Debug::Endl();
	}

    //========================================================================================================
    //========================================================================================================
    void  Debug::Error( const std::string _message )
	{
	    Get() << Severity::error << _message << Debug::Endl();
	}

    //========================================================================================================
    //========================================================================================================
	void  Debug::Highlight( const std::string _message )
	{
	    Get() << Severity::highlight << _message << Debug::Endl();
	}

	//========================================================================================================
	//========================================================================================================
	Debug& Debug::Log() { Get() << Debug::Severity::log;		return Get(); }
	Debug& Debug::Warning() { Get() << Debug::Severity::warning;	return Get(); }
	Debug& Debug::Error() { Get() << Debug::Severity::error;		return Get(); }
	Debug& Debug::Highlight() { Get() << Debug::Severity::highlight;	return Get(); }

	//========================================================================================================
	//========================================================================================================
	void Debug::Clear() { Get().mLogBuffer.clear(); }

    //========================================================================================================
    // Returns a hours:minuts:seconds ex: 3783s = 01:02:03
    //========================================================================================================
    std::string Debug::SecondsToString( const double _seconds )
    {
        std::stringstream ss;
        unsigned remainingSeconds = static_cast<unsigned>( _seconds );

        const unsigned hours = remainingSeconds / 3600;
        remainingSeconds -= hours * 3600;
        ss << '[';
        if( hours < 10 )
        {
            ss << '0';
        }
        ss << hours << ':';

        const unsigned minuts = remainingSeconds / 60;
        remainingSeconds -= minuts * 60;
        if( minuts < 10 )
        {
            ss << 0;
        }
        ss << minuts << ':';

        const unsigned seconds = remainingSeconds;
        if( seconds < 10 )
        {
            ss << '0';
        }
        ss << seconds << ']';
        return ss.str();
    }
}