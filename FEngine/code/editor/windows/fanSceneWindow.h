#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	namespace scene { class Entity; }

	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class SceneWindow : public Window {
		public:
			SceneWindow();

			void Draw() override;			
			void NewEntityModal();
			void RenameEntityModal();

		private:
			std::array<char, 64> m_textBuffer;
			scene::Entity * m_lastEntityRightClicked = nullptr;

			void R_DrawSceneTree(scene::Entity * _entity, scene::Entity*& _entityRightClicked);
		};
	}
}
