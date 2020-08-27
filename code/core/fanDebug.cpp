#include "core/fanDebug.hpp"

#include <iostream>
#include "network/singletons/fanTime.hpp"

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
		item.time = Time::ElapsedSinceStartup();
		mLogBuffer.push_back( item );

		// stdio
		std::cout << Time::SecondsToString( item.time );
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
			assert( false );
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
}