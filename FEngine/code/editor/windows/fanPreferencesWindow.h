#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class PreferencesWindow : public EditorWindow {
	public:
		PreferencesWindow();

	protected:
		void OnGui() override;
	};
}
