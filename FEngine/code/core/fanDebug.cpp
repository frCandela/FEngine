#include "fanIncludes.h"

#include "core/fanDebug.h"

namespace fan {
	void Debug::Init() {
		m_currentSeverity = Severity::log;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Log(const std::string _message, const Severity & _severity) {
		Debug & instance = Get();
		instance << _severity << _message << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Debug::Clear() {
		Get().m_logBuffer.clear();
		fan::Debug::Log("clear", Severity::highlight);
	}
}