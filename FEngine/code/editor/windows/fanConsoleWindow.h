#pragma once

#include "editor/windows/fanWindow.h"
#include "core/fanDebug.h"

namespace editor {
	//================================================================================================================================
	//================================================================================================================================
	class ConsoleWindow : public editor::Window {

	public:
		void Draw() override;

	private:
		ImVec4 GetSeverityColor(const fan::Debug::Severity & _severity);

		static const uint32_t s_inputBufferSize = 256;
		
		char						m_inputBuffer[s_inputBufferSize];
		bool						m_scrollDown;
	};
}