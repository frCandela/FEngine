#include "fanIncludes.h"

#include "core/fanDebug.h"
#include "core/fanTime.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	void Debug::Init() {
		m_currentSeverity = Severity::log;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Flush() {
		// Push the log into the buffer
		LogItem item;
		item.message = m_stringstream.str();
		item.severity = m_currentSeverity;
		item.time = Time::ElapsedSinceStartup();
		m_logBuffer.push_back( item );

		std::cout << Time::SecondsToString(item.time);

		// stdio
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
		std::cout << " " << m_stringstream.str() << std::endl;
		m_stringstream.str(""); // clear
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Log(const std::string _message, const Severity & _severity) {
 		Get() << _severity << _message << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Clear() {
		Get().m_logBuffer.clear();
		Log("clear", Severity::highlight);
	}
}