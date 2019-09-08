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

		private:
			std::array<char, 64> m_textBuffer;

			void R_DrawSceneTree(scene::Entity * _entity);
		};
	}
}
