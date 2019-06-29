#pragma once

namespace editor {
	class MainMenuBar {
	public:
		MainMenuBar();

		void Draw();
	private:
		bool m_showImguiDemoWindow;
	};
}