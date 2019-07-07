#pragma once

#include "editor/windows/fanWindow.h"

namespace editor {
	class SceneWindow : public Window {
	public:
		SceneWindow();

		void Draw() override;
		void NewGameobjectModal();

	private:
		std::array<char, 64> m_textBuffer;
	};
}
