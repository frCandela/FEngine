#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class SceneWindow : public Window {
		public:
			SceneWindow();

			void Draw() override;
			void NewentityModal();

		private:
			std::array<char, 64> m_textBuffer;
		};
	}
}
