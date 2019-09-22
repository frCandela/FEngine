#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class RenderWindow : public EditorWindow {
	public:
		RenderWindow();

	protected:
		void OnGui() override;
	};
}