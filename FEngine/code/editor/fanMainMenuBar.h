#pragma once

namespace editor {
	class MainMenuBar {
	public:
		MainMenuBar();
		~MainMenuBar();

		void Initialize();
		void Draw();
	private:
		bool m_showImguiDemoWindow;
	};
}