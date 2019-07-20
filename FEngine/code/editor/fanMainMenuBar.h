#pragma once

namespace editor {
	class MainMenuBar {
	public:
		MainMenuBar();
		~MainMenuBar();

		void Initialize();
		void Draw();

		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
	private:
		bool m_showImguiDemoWindow;
		bool m_showAABB;
		bool m_showWireframe;
	};
}