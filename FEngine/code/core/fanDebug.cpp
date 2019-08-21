#include "fanIncludes.h"

#include "core/fanDebug.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	void Debug::Log(const std::string _message, const Severity & _severity) {
		std::cout << _message << std::endl;
		GetInstance().m_logBuffer.push_back({});

		LogItem & item = GetInstance().m_logBuffer[GetInstance().m_logBuffer.size() - 1];
		item.message = _message;
		item.severity = _severity;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Clear() {
		GetInstance().m_logBuffer.clear();
		fan::Debug::Log("clear", Severity::highlight);
	}
}