#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	namespace scene { 
		class Scene;
		class Entity; 
	}

	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class SceneWindow : public Window {
		public:
			SceneWindow();
			~SceneWindow();

			void NewEntityModal();
			void RenameEntityModal();

		protected:
			void OnGui() override;

		private:
			std::array<char, 64> m_textBuffer;
			scene::Entity * m_lastEntityRightClicked = nullptr;
			bool m_expandSceneHierarchy = false;

			void OnSceneLoad(scene::Scene * /*_scene*/ ) { m_expandSceneHierarchy = true;  }

			void R_DrawSceneTree(scene::Entity * _entity, scene::Entity*& _entityRightClicked);
		};
	}
}
