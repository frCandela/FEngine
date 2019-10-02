#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	class Scene;
	class Entity;	

	//================================================================================================================================
	//================================================================================================================================
	class SceneWindow : public EditorWindow {
	public:
		Signal< Entity* > onSelectEntity;

		SceneWindow( Scene * _scene );
		~SceneWindow();

		// Callbacks
		void OnEntitySelected( Entity* _entity ) { m_entitySelected = _entity; }
		void OnSceneLoad( Scene * /*_scene*/ ) { m_expandSceneHierarchy = true; }

	protected:
		void OnGui() override;

	private:
		Scene * m_scene;
		Entity* m_entitySelected;

		char m_textBuffer[32];
		Entity * m_lastEntityRightClicked = nullptr;
		bool m_expandSceneHierarchy = false;

		void NewEntityModal();
		void RenameEntityModal();
		void R_DrawSceneTree(Entity * _entity, Entity*& _entityRightClicked);
	};
}
