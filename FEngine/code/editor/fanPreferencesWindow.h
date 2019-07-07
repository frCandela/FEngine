#pragma once

#include "editor/fanWindow.h"

namespace editor {
	class PreferencesWindow : public Window {
	public:
		PreferencesWindow();

		void Draw() override;
	};
}
