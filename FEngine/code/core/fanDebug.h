#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Debug : public Singleton<fan::Debug> {
	public:
		enum Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		static void Log(const std::string _message, const Severity & _severity = Severity::log );
		static void Clear();

		struct LogItem {
			Severity	severity;
			std::string message;
		};

		const std::vector< LogItem >& GetLogBuffer() const { return m_logBuffer;  }

	private:
		std::vector< LogItem >		m_logBuffer;
	};
}