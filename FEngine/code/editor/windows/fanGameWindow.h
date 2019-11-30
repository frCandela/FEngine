#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class GameWindow : public EditorWindow {
	public:
		Signal< glm::ivec2 > onSizeChanged;

		GameWindow();
		glm::ivec2 GetSize() const { return m_size; }

	protected:
		void OnGui() override;

	private:
		glm::ivec2 m_size = glm::ivec2(0,0);
	};

}