#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	struct EditorGrid;
	class Scene;
	class EditorWindow;
	class EditorSelection;

	//================================================================================================================================
	//================================================================================================================================
	class MainMenuBar
	{
	private:
		//================================================================
		//================================================================
		struct FPSCounter
		{
			int count = 0;
			float sum = 0.f;
			float fps = 0.f;
		};

	public:
		enum CurrentScene { CLIENTS, SERVER };
		Signal< int > onSetScene;

		Signal< std::string > onSaveScene;
		Signal< std::string > onNewScene;
		Signal< std::string > onOpenScene;
		Signal<> onReloadShaders;
		Signal<> onReloadIcons;
		Signal<> onExit;

		MainMenuBar( EditorSelection& _editorSelection );
		~MainMenuBar();

		void SetScene( Scene* _scene ) { m_scene = _scene; }
		void SetGrid( EditorGrid* _editorGrid ) { m_editorGrid = _editorGrid; }

		void SetWindows( std::vector< EditorWindow* > _editorWindows );

		void Draw();

		bool ShowHull() const { return m_showHull; }
		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
		bool ShowNormals() const { return m_showNormals; }

	private:
		Scene* m_scene;
		EditorGrid* m_editorGrid;
		EditorSelection& m_editorSelection;

		std::vector< EditorWindow* > m_editorWindows;

		CurrentScene m_currentScene = CurrentScene::CLIENTS;

		bool m_showImguiDemoWindow;
		bool m_showHull;
		bool m_showAABB;
		bool m_showWireframe;
		bool m_showNormals;

		bool m_openNewScenePopupLater = false;
		bool m_openLoadScenePopupLater = false;
		bool m_openSaveScenePopupLater = false;

		FPSCounter m_fpsCounter;

		// Temporary buffers
		std::filesystem::path m_pathBuffer;
		std::set < std::string > m_sceneExtensionFilter;

		void DrawModals();
		void New();
		void Open();
		void Reload();
		void Save();
		void SaveAs();
	};
}