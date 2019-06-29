#pragma once

namespace vk {
	class Renderer;
}

namespace editor {
	class MainMenuBar;
	class RenderWindow;
}

namespace fan {
	class Engine {
	public:
		Engine();

		void Run();
		void Exit();

		static Engine & GetEngine() { return * ms_engine; }

		editor::MainMenuBar &  GetMainMenuBar()		{ return * m_mainMenuBar; }
		editor::RenderWindow & GetRenderWindow()	{ return * m_renderWindow; }

	private:
		editor::MainMenuBar * m_mainMenuBar;
		editor::RenderWindow * m_renderWindow;

		static Engine * ms_engine;

		bool m_applicationShouldExit;

		void DrawUI();

	};
}
