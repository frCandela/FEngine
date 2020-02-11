#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class Scene;
	class Gameobject;

	//================================================================================================================================
	//================================================================================================================================
	class SceneWindow : public EditorWindow
	{
	public:
		Signal< Gameobject* > onSelectGameobject;

		SceneWindow();
		~SceneWindow();

		void SetScene( Scene* _scene ) { m_scene = _scene; }

		// Callbacks
		void OnGameobjectSelected( Gameobject* _gameobject ) { m_gameobjectSelected = _gameobject; }
		void OnExpandHierarchy( Scene* /*_scene*/ ) { m_expandSceneHierarchy = true; }

	protected:
		void OnGui() override;

	private:
		Scene* m_scene;
		Gameobject* m_gameobjectSelected;

		std::filesystem::path m_pathBuffer;
		char m_textBuffer[ 32 ];
		Gameobject* m_lastGameobjectRightClicked = nullptr;
		bool m_expandSceneHierarchy = false;

		void NewGameobjectModal();
		void RenameGameobjectModal();
		void ExportToPrefabModal();

		void R_DrawSceneTree( Gameobject* _gameobject, Gameobject*& _gameobjectRightClicked );
	};
}
