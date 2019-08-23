#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Debug : public Singleton<fan::Debug> {
	public:
		enum class Severity { log = 0, highlight = 1, warning = 2, error = 3 };

		static void Log(const std::string _message, const Severity & _severity = Severity::log );
		static void Clear();

		template <typename T>
		friend Debug& operator<<(Debug& _logger, T _msg) {
			Get().m_stringstream << _msg;
			return _logger;
		}

 		friend Debug& operator<<(Debug& _logger, Severity _severity) {
			Get().m_currentSeverity = _severity;
 			return _logger;
 		}

		friend Debug& operator<<(Debug& _logger, std::ostream&(*_pManip)(std::ostream&)) {
			assert(_pManip == std::endl);
			Get().Flush();
			return _logger;
		}

		struct LogItem {
			Severity	severity;
			std::string message;
		};

		const std::vector< LogItem >& GetLogBuffer() const { return m_logBuffer;  }

	private:		
		Severity				m_currentSeverity;
		std::stringstream		m_stringstream;

		std::vector< LogItem >	m_logBuffer;

		void Init() override;
		void Flush() {
			Debug & instance = Get();
			
			// Push the log into the buffer
			LogItem item;
			item.message = m_stringstream.str();
			item.severity = m_currentSeverity;
			instance.m_logBuffer.push_back( item );


			// stdio
			switch (m_currentSeverity)
			{
			case Severity::log: {
				std::cout << "[__LOG__]";
			} break;
			case Severity::warning: {
				std::cout << "[WARNING]";
			} break;
			case Severity::error: {
				std::cout << "[_ERROR_]";
			} break;
			case Severity::highlight: {
				std::cout << "[_HIGH__]";
			} break;
			default:
				assert(false);
				break;
			}
			std::cout << "[00:00:00]"; // Timestamp
			std::cout << " " << m_stringstream.str() << std::endl;
			m_stringstream.str(""); // clear
		}
	};
}

