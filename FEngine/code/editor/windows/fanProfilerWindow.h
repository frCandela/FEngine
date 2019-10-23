#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class ProfilerWindow : public EditorWindow {
	public:
		ProfilerWindow();
	protected:
		void OnGui() override;

	private:
	};

}