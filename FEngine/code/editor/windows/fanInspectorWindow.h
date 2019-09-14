#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	namespace scene {
		class Transform;
		class Component;
		class Camera;
		class Model;
		class FPSCamera;
		class Material;
	}

	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		class InspectorWindow : public editor::Window {
		public:
			InspectorWindow();

			void Draw() override;

		private:
			void NewComponentPopup();
		};
	}
}
