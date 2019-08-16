#pragma once

#include "editor/windows/fanWindow.h"

namespace editor {
	//================================================================================================================================
	//================================================================================================================================
	class ConsoleWindow : public editor::Window {

	public:
		enum Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		void Log(const std::string _message, const Severity & _severity );
		void Draw() override;

	private:
		void Clear();
		ImVec4 GetSeverityColor(const Severity & _severity);
		
		struct LogItem {
			Severity	severity;
			std::string message;
		};

		static const uint32_t s_inputBufferSize = 256;

		std::vector< LogItem >		m_logBuffer;
		char						m_inputBuffer[s_inputBufferSize];
		bool						m_scrollDown;
	};
}