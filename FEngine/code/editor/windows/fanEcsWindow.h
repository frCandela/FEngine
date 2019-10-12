#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsWindow : public EditorWindow {
	public:
		EcsWindow();
	protected:
		void OnGui() override;

	private:
	};

}