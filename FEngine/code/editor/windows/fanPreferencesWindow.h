#pragma once

#include "editor/windows/fanWindow.h"

namespace editor {
	class PreferencesWindow : public Window {
	public:
		PreferencesWindow();

		void Draw() override;
	};
}
