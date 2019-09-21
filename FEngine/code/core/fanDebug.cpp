#include "fanGlobalIncludes.h"

#include "core/fanDebug.h"
#include "core/fanTime.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Debug::Debug() {
		m_currentSeverity = Severity::log;
		m_currentType =		Type::other;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Flush() {
		// Push the log into the buffer
		LogItem item;
		item.message = m_stringstream.str();
		item.severity = m_currentSeverity;
		item.type = m_currentType;
		item.time = Time::ElapsedSinceStartup();
		m_logBuffer.push_back( item );
		
		// stdio
		std::cout << Time::SecondsToString(item.time);
		switch (m_currentSeverity)
		{
		case Severity::log: {
			std::cout << "[LOG]";
		} break;
		case Severity::warning: {
			std::cout << "[WARNING]";
		} break;
		case Severity::error: {
			std::cout << "[ERROR]";
		} break;
		case Severity::highlight: {
			std::cout << "[HIGH]";
		} break;
		default:
			assert(false);
			break;
		}
		std::cout << " " << m_stringstream.str().c_str() << std::endl;
		m_stringstream.str(""); // clear
		m_currentSeverity = Severity::log;
		m_currentType = Type::other;
		onNewLog.Emmit(item);
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Debug::Log( const std::string _message ){
		Get() << Severity::log << _message << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Debug::Warning( const std::string _message ){
		Get() << Severity::warning << _message << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Debug::Error( const std::string _message ){
		Get() << Severity::error << _message << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Debug::Highlight( const std::string _message ){
		Get() << Severity::highlight << _message << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Clear() {
		Get().m_logBuffer.clear();		
	}
}