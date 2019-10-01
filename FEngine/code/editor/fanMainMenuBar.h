#pragma once

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class MainMenuBar {
	private:
		//================================================================
		//================================================================
		struct FPSCounter {
			int count = 0;
			float sum = 0.f;
			float fps = 0.f;
		};

	public:
		MainMenuBar();
		~MainMenuBar();

		void Initialize();
		void Draw();

		bool ShowHull() const { return m_showHull; }
		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
		bool ShowNormals() const { return m_showNormals; }
	private:
		bool m_showImguiDemoWindow;
		bool m_showHull;
		bool m_showAABB;
		bool m_showWireframe;
		bool m_showNormals;

		void ProcessKeyboardShortcuts();
		void DrawModals();
		void New();
		void Open();
		void Save();
		void SaveAs();

		bool m_openNewScenePopupLater = false;
		bool m_openLoadScenePopupLater = false;
		bool m_openSaveScenePopupLater = false;

		FPSCounter m_fpsCounter;

		// Temporary buffers
		std::fs::path m_pathBuffer;
		int m_extensionIndexBuffer;
		std::set < std::string > m_sceneExtensionFilter;
	};
}