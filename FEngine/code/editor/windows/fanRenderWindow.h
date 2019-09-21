#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class RenderWindow : public Window {
		public:
			RenderWindow();

		protected:
			void OnGui() override;
		};
	}
}
