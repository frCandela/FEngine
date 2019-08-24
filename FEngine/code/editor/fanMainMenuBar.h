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
		bool ShowNormals() const { return m_showNormals; }
	private:
		bool m_showImguiDemoWindow;
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

		// Temporary buffers
		std::fs::path m_pathBuffer;
		int m_extensionIndexBuffer;
		std::set < std::string > m_sceneExtensionFilter;
	};
}