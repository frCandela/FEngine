#pragma once

#include "editor/fanWindow.h"

namespace editor {
	class RenderWindow : public Window {
	public:
		RenderWindow();

		void Draw() override;
	};
}
