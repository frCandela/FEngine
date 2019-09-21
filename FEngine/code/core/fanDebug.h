#pragma once

#include "core/fanSingleton.h"
#include "core/fanSignal.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Debug : public Singleton<Debug> {
		friend class Singleton<Debug>;
	
	public:
		enum class Code		{ endl };
		enum class Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		enum class Type { render, scene, game, other };
		struct LogItem {
			Severity	severity;
			Type		type;
			std::string message;
			double time;
		};

		fan::Signal<LogItem> onNewLog;

		static Code Endl() { return Code::endl;  }
		static void Log		( const std::string _message, const Severity & _severity );
		static void Log		( const std::string _message );
		static void Warning	( const std::string _message );
		static void Error	( const std::string _message );
		static void Highlight	( const std::string _message );
		static void Clear();
		static void Break() { __debugbreak(); }
		const std::vector< LogItem >& GetLogBuffer() { return m_logBuffer;  }

	protected:
		Debug();

	private:		
		Severity				m_currentSeverity;
		Type					m_currentType;
		std::stringstream		m_stringstream;
		std::vector< LogItem >	m_logBuffer;
		void Flush();

	public:
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, Severity _severity){	// Set the severity of the current log
			_logger.m_currentSeverity = _severity;
			return _logger;
		}

		friend Debug& operator<<(Debug& _logger, Type _type){	// Set the type of the current log
			_logger.m_currentType = _type;
			return _logger;
		}

		//================================================================================================================================
		template <typename T>
		friend Debug& operator<<(Debug& _logger, T _msg){	// Appends a value to the current log
			_logger.m_stringstream << _msg;
			return _logger;
		}
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, Code _code ){	// Special case of Debug::Get() << std::endl 			
			_logger.Flush();			
			return _logger;
		}
	};
}

