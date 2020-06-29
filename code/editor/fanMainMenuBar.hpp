#pragma once

#include "ecs/fanSignal.hpp"
#include <filesystem>
#include <set>

namespace fan
{
	struct EditorGrid;
	class EcsWorld;
	class EditorWindow;
	struct EditorSelection;

	//================================================================================================================================
	// top main menu bar of the editor
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
		Signal< std::string > onSaveScene;
		Signal< std::string > onNewScene;
		Signal< std::string > onOpenScene;
		Signal<> onReloadShaders;
		Signal<> onReloadIcons;
		Signal<> onExit;

		MainMenuBar();
		~MainMenuBar();

		void SetWindows( std::vector< EditorWindow* > _editorWindows );

		void Draw( EcsWorld& _world );

		bool ShowHull() const { return m_showHull; }
		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
		bool ShowNormals() const { return m_showNormals; }
		bool ShowLights() const { return m_showLights; }

		void Open( EcsWorld& _world );
		void Reload( EcsWorld& _world );
		void Save( EcsWorld& _world );
	private:
		std::vector< EditorWindow* > m_editorWindows;

		bool m_showImguiDemoWindow;
		bool m_showHull;
		bool m_showAABB;
		bool m_showWireframe;
		bool m_showNormals;
		bool m_showLights;

		bool m_openNewScenePopupLater = false;
		bool m_openLoadScenePopupLater = false;
		bool m_openSaveScenePopupLater = false;

		FPSCounter m_fpsCounter;

		// Temporary buffers
		std::filesystem::path m_pathBuffer;
		std::set < std::string > m_sceneExtensionFilter;

		void DrawModals( EcsWorld& _world );
		void New( EcsWorld& _world );

		void SaveAs();
	};
}