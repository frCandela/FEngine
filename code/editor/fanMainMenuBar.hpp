#pragma once

#include "ecs/fanSignal.hpp"
#include <filesystem>
#include <set>

namespace fan
{
	class EcsWorld;
	class EditorWindow;

	//========================================================================================================
	// top main menu bar of the editor
	//========================================================================================================
	class MainMenuBar
	{
	public:
		Signal<> mOnReloadShaders;
		Signal<> mOnReloadIcons;
		Signal<> mOnExit;

		MainMenuBar();
		~MainMenuBar();

		void SetWindows( std::vector< EditorWindow* > _editorWindows );

		void Draw( EcsWorld& _world );

		bool ShowHull() const { return mShowHull; }
		bool ShowAABB() const { return mShowAABB; }
		bool ShowWireframe() const { return mShowWireframe; }
		bool ShowNormals() const { return mShowNormals; }
		bool ShowLights() const { return mShowLights; }
        bool ShowUiBounds() const { return mShowUiBounds; }

		void Open( EcsWorld& _world );
		void Reload( EcsWorld& _world );
		void Save( EcsWorld& _world );
	private:
		std::vector< EditorWindow* > mEditorWindows;

		bool mShowImguiDemoWindow = true;
		bool mShowHull            = false;
		bool mShowAABB            = false;
		bool mShowWireframe       = false;
		bool mShowNormals         = false;
		bool mShowLights   = false;
        bool mShowUiBounds = false;

		bool mOpenNewScenePopupLater  = false;
		bool mOpenLoadScenePopupLater = false;
		bool mOpenSaveScenePopupLater = false;

		// Temporary buffers
		std::filesystem::path    mPathBuffer;
		std::set < std::string > mSceneExtensionFilter;

		void DrawModals( EcsWorld& _world );
		void New( EcsWorld& _world );

		void SaveAs();
	};
}