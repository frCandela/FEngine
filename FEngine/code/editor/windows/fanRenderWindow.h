#pragma once

#include "editor/windows/fanWindow.h"

namespace editor {
	class RenderWindow : public Window {
	public:
		RenderWindow();

		void Draw() override;
	};
}
