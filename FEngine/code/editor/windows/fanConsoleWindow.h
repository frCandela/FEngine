#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class ConsoleWindow : public editor::Window {

		public:
			ConsoleWindow();
			void Draw() override;

		private:
			ImVec4	GetSeverityColor(const fan::Debug::Severity & _severity);
			void	OnNewLog() { m_scrollDown = true; }

			static const uint32_t s_inputBufferSize = 256;

			char						m_inputBuffer[s_inputBufferSize];
			bool						m_scrollDown;
		};
	}
}