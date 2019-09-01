#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class PreferencesWindow : public Window {
		public:
			PreferencesWindow();

			void Draw() override;
		};
	}
}
