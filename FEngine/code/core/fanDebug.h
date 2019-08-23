#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Debug : public Singleton<fan::Debug> {
	public:

		enum class Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		struct LogItem {
			Severity	severity;
			std::string message;
			double time;
		};

		static void Log(const std::string _message, const Severity & _severity = Severity::log );
		static void Clear();
		static const std::vector< LogItem >& GetLogBuffer() { return Get().m_logBuffer;  }

	private:		
		Severity				m_currentSeverity;
		std::stringstream		m_stringstream;
		std::vector< LogItem >	m_logBuffer;

		void Init() override;
		void Flush();

	public:
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, Severity _severity){	// Set the severity of the current log
			_logger.m_currentSeverity = _severity;
			return _logger;
		}

		//================================================================================================================================
		template <typename T>
		friend Debug& operator<<(Debug& _logger, T _msg){	// Appends a value to the current log
			_logger.m_stringstream << _msg;
			return _logger;
		}
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, std::ostream&(*_pManip)(std::ostream&)){	// Usage : Debug::Get() << std::endl flushe
			assert(_pManip == std::endl);
			_logger.Flush();
			return _logger;
		}
	};
}

