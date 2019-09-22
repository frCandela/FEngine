#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	class Scene;
	class Entity;	

	//================================================================================================================================
	//================================================================================================================================
	class SceneWindow : public EditorWindow {
	public:
		SceneWindow();
		~SceneWindow();

		void NewEntityModal();
		void RenameEntityModal();

	protected:
		void OnGui() override;

	private:
		std::array<char, 64> m_textBuffer;
		Entity * m_lastEntityRightClicked = nullptr;
		bool m_expandSceneHierarchy = false;

		void OnSceneLoad(Scene * /*_scene*/) { m_expandSceneHierarchy = true; }

		void R_DrawSceneTree(Entity * _entity, Entity*& _entityRightClicked);
	};
}
